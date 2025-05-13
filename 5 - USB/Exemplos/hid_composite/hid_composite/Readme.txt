Algumas considerações sobre os projetos aqui apresentados

1. O projeto hid_composite é uma adaptação do projeto original do sdk contido em: C:\Users\fborg\.pico-sdk\sdk\2.1.1\lib\tinyusb\examples\device\hid_composite
2. A adaptação foi feita no CMakeLists.txt para que todos os arquivos ficassem na pasta do projeto hid_composite, também foi removido a parte para múltiplas plataforma (Esp32, etc). Este projeto têm as funcionalidades simultaneamente de teclado, mouse, controle de consumidor (Consumer Control) e gamepad simplificadas. 
3. O projeto hid_composite_consummer_control, é uma adaptação do projeto hid_composite, customizada para multimídia (aumentar o volume no PC) ao pressionar o botão GPIO 5 e diminuir ao pressionar o botão no GPIO 6.
6. Há um relatório para cada implementação 