# Quarto-Hospitalar-IoT

![Maquete quarto iot](https://github.com/zw0s/Quarto-Hospitalar-IoT/blob/main/quarto_hosp_iot/quartoiot.jpg)
  Esse projeto foi para a disciplina Engenharia Unificada II da UFABC, o intuito foi recriar um quarto hospitalar inteligente e que todos os dispositivos pudessem ser controlados remotamanete atráves de um site/app.

## Como ele funciona?

  O quarto seria monitorado por um profissional da área da saúde que tem o poder de controlar todos os dispositivos do quarto do paciente remotamente atráves de um simples clique, então se ele precisasse acender/apagar as luzes, ajustar a cama e controlar temperatura e umidade do quarto poderia ser feito com um clique, assim facilitando sua vida.
  Outra funcionalidade interessante é que atráves de sensores piezoelétricos instalados no chão do quarto, é possível determinar se o paciente se movimentou e principalmente se houve uma queda! Essa informação é enviada direto para a interface do profissional sem que ele precise efetivamente monitorar com câmeras ou presencialmente.

## Materiais do projeto
- O projeto foi feito utilizando uma ESP32 para fazer o controle de gerar o webserver que poderia ser acessado atráves de um url.
- Servo motores para controlar a cama.
- Sensor piezoelétrico no chão.
- Leds espalhados pelo quarto.
- Sensor dht11 de umidade e temperatura.
