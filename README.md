# ESP32_Sistema_Casa_Inteligente

Projeto prático para simular uma casa inteligente usando ESP32 para a disciplina Microcontroladores do curso Ciências da Computação 7º semestre do IFCE Maracanaú.

## TODO - Projeto Prático: Sistema de Casa Inteligente (28 e 30/05) - Relatório 01/06

- [x] Usar o sensor de distância para medir o nível de água da caixa d’água
- [x] Usar o LED RGB para mudar a intensidade (cor) de acordo com o nível
- [x] Criar um controle de acesso usando o **RFID RDM6300**
  - [ ] Acionar o relé para um usuário liberado
  - [ ] Simular alguma "carga" conectada ao relé (fechadura)
- [x] Todo o projeto deve enviar e receber comandos do **Telegram**
- [x] Salvar todo o estado do ambiente com **LittleFS**
  - [x] Logs de usuários autorizados e não autorizados
  - [x] Logs de medições de distância
- [ ] Deve ser possível **cadastrar/remover usuários pelo Telegram**
