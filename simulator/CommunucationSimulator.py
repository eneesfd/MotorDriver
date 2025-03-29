import serial
import time
import argparse
import logging
import serial.tools.list_ports
from decimal import Decimal, InvalidOperation

# Log yapılandırması
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class MotorControllerSimulator:
    def __init__(self, port, baudrate=115200):
        """Motor kontrol simulatörünü başlat."""
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        
        # Varsayılan parametre değerleri
        self.parameters = {
            "PL": {
                1: 25.0,         # Peak current value (Amps)
                2: 500           # Peak current duration (milliseconds)
            },
            "CL": {
                1: 15.0          # Continuous current (Amps)
            },
            "US": {
                1: 95.0,         # PWM output duty cycle (%)
                2: 123456789123456789123456789123456789123456789123456789.0,  # Peak integral limit
                4: 48.0          # Max voltage BV (Volts)
            }
        }

    def list_available_ports(self):
        """Mevcut seri portları listele."""
        ports = serial.tools.list_ports.comports()
        logger.info("Mevcut seri portlar:")
        for port in ports:
            logger.info(f"  Port: {port.device}, Açıklama: {port.description}")
        return ports

    def connect(self):
        """Seri porta bağlan."""
        try:
            # Önce mevcut portları listele
            self.list_available_ports()
            
            self.serial = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1
            )
            logger.info(f"{self.port} portuna {self.baudrate} baud ile bağlandı")
            return True
        except serial.SerialException as e:
            logger.error(f"{self.port} portuna bağlanılamadı: {e}")
            return False

    def run(self):
        """Komutları dinle ve yanıtları gönder."""
        if not self.serial or not self.serial.is_open:
            if not self.connect():
                return
        
        logger.info("Motor kontrol simulatörü çalışıyor. Komutlar bekleniyor...")
        
        buffer = bytearray()
        
        while True:
            try:
                # Veri var mı kontrol et
                if self.serial.in_waiting > 0:
                    # Byte oku ve buffer'a ekle
                    data = self.serial.read(1)
                    buffer.extend(data)
                    
                    # Eğer CR karakteri (0x0D) görürsen komutu işle
                    if data == b'\r':
                        command_str = buffer[:-1].decode('ascii')  # CR karakterini çıkar
                        logger.info(f"Alınan komut: {command_str}")
                        
                        self.process_command(command_str)
                        
                        # Buffer'ı temizle
                        buffer = bytearray()
                
                # CPU yükünü azaltmak için küçük bir gecikme
                time.sleep(0.01)
                
            except serial.SerialException as e:
                logger.error(f"Seri haberleşme hatası: {e}")
                # Yeniden bağlantı dene
                self.serial.close()
                time.sleep(1)
                self.connect()
            except Exception as e:
                logger.error(f"Beklenmeyen hata: {e}")
                buffer = bytearray()  # Hata durumunda buffer'ı temizle
    
    def process_command(self, command_str):
        """Alınan komutu işle ve yanıt gönder."""
        try:
            # Komut formatı: PL[1], CL[1], US[1] vb.
            # Komut tipini ve ID'yi ayır
            cmd_type = command_str[:2]  # İlk iki karakter komut tipi
            
            # Köşeli parantez içindeki ID'yi çıkar
            if '[' in command_str and ']' in command_str:
                cmd_id = int(command_str[command_str.index('[') + 1:command_str.index(']')])
                
                # Değeri bul ve yanıt gönder
                if cmd_type in self.parameters and cmd_id in self.parameters[cmd_type]:
                    value = self.parameters[cmd_type][cmd_id]
                    self.send_response(value)
                else:
                    logger.warning(f"Bilinmeyen komut veya ID: {command_str}")
                    # Hata durumunda bir yanıt gönder
                    self.send_response(0)
            else:
                logger.warning(f"Geçersiz komut formatı: {command_str}")
        
        except Exception as e:
            logger.error(f"Komut işleme hatası {command_str}: {str(e)}")
    
    def send_response(self, value):
        """Seri port üzerinden yanıt gönder."""
        try:
            # Değeri string'e dönüştür - bilimsel gösterimi engelleyerek
            formatted_value = ""
            
            if isinstance(value, (int, float)):
                # Sayıyı bilimsel gösterim olmadan formatlama
                if abs(value) >= 1e16:
                    # Çok büyük sayılar için saf string dönüşümü
                    formatted_value = f"{value:.0f}"
                elif isinstance(value, float) and value.is_integer():
                    # Tam sayı değerli float için ondalık kısmı kaldır
                    formatted_value = f"{int(value)}"
                else:
                    # Normal sayılar
                    formatted_value = str(value).rstrip('0').rstrip('.') if '.' in str(value) else str(value)
            else:
                formatted_value = str(value)
            
            response = f"{formatted_value};\r\n"
            logger.info(f"Yanıt gönderiliyor: {response.strip()}")
            self.serial.write(response.encode('ascii'))
            
        except Exception as e:
            # Hata durumunda loglama yap ve basit bir yanıt gönder
            logger.error(f"Yanıt formatlamada hata: {str(e)}")
            fallback_response = f"{value};\r\n"
            self.serial.write(fallback_response.encode('ascii'))


if __name__ == "__main__":
    # Komut satırı argümanlarını işle
    parser = argparse.ArgumentParser(description='Motor Kontrol Simulatörü')
    parser.add_argument('--port', required=True, help='Kullanılacak seri port (örn. COM3, /dev/ttyUSB0)')
    parser.add_argument('--baudrate', type=int, default=115200, help='Seri haberleşme için baud rate')
    args = parser.parse_args()

    # Simulatörü oluştur ve çalıştır
    simulator = MotorControllerSimulator(args.port, args.baudrate)
    
    # Mevcut portları listele (bağlantı denemeden önce referans için)
    simulator.list_available_ports()
    
    simulator.run()