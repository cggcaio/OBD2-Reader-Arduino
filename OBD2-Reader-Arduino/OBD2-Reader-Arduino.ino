
// Author: Felipe Adalberto Farinelli
// Date: 08/26/2019
// Title: OBD2 RPM Scanner
// Version: 0

// Inclusão da biblioteca do CAN.
#include <CAN.h>


void setup() {
  // Inicilização do debugger serial do arduino
  Serial.begin(9600);
  Serial.println("OBD2 RPM Scanner!");

  // O barramento CAN de diagnostico pode operar em duas velocidades 500 ou 250 kbps. Na maioria dos carros é 500, inclusive no Gol. Logo:
  // se o barramento nao for iniciado em 500 * 10^3 bps (500kbps),
  if (!CAN.begin(500E3)) {
    // Então a inicialização falhou!!
    Serial.println("Starting CAN failed!");
    // E o programa morre, entrando nesse loop infinito para evitar que "bobagens" sejam mandadas ao carro e causem problemas
    while (1);
  } else {
    Serial.println("CAN-bus started successfully!");
  }
  // CAso a inicilização tenha sido executada com sucesso (Que nesse caso, quase 100% dos casos será,a não ser que tenha conexões erradas do arduino para o shield CAN), então
  // Há inúmeras mensagens circulando no barramento do veículo, mas é a resposta ao frame 0x7DF que nos interessa. logo:
  CAN.filter(0x7e8);
}

void loop() {
  // Variáveis para armazenar os dados vindos da resposta do veículo.
  byte D7, D6, D5, D4, D3, D2, D1, D0;
  float RPM;
  float VELO;
  float CONS;
  float TESTE;
  
  // Aqui, você está começando a estruturar um frame CAN com ID 0x7DF e DLC de 8 bytes.
  CAN.beginPacket(0x7df, 8);
  // Mas quantos dos 8 bytes são úteis pro veiculo? Apenas 2: o serviço e o PID que você quer.
  CAN.write(0x02); // number of additional bytes
  // Logo, você quer ler os dados congelados na ECU (ou seja, os últimos atualizados): Serviço 01 (https://en.wikipedia.org/wiki/OBD-II_PIDs)
  CAN.write(0x01);
  // E o dado que você quer ler, é o RPM: PID = 0x0C
  CAN.write(0x40);
  // Seu frame de requisição está pronto, então, já pode finalizar o pacote:
  CAN.endPacket();

  // Ao chamar a função endPacket, você envia pelo barramento CAN o pacote que você montou. O que resta agora?
  // Aguardar o pacote chegar! Ou seja, 
  // enquanto a resposta não chegar, você aguarda! Lembre-se: A resposta vem no frame 0x7e8. O controlador vai receber apenas esse frame porque você disse que queria apenas
  // ele em void setup(). Normalmente, há outras mensagens circulando no barramento do veiculo!
  
  while (CAN.parsePacket() == 0);
  // Opa, se chegamos até aqui, quer dizer que temos uma resposta do carro. Mas é mesmo a resposta que você esperava?
  // Vamos receber todos os dados do campo DATA da mensagem CAN.
  // Você pediu o RPM ao carro, concorda? Então:
  D7 = CAN.read(); // D7 vai armazenar o número de bytes da requisição, que são 4:
  D6 = CAN.read(); // O serviço acrescido de 0x40 unidades. Logo: D6 = 0x01 + 0x40 = 0x41;
  D5 = CAN.read(); // O PID que você pediu: D5 = 0x0C
  D4 = CAN.read(); // O primeiro byte do RPM (ou A) D4 = A
  D3 = CAN.read(); // O segundo byte do RPM (ou B) D3 = B
  D2 = CAN.read(); // Sua resposta está pronta, logo: D2 = 0;
  D1 = CAN.read(); // D1 = 0;
  D0 = CAN.read(); // D0 = 0;
  // teste alteracao
  // Se temos as informações úteis do RPM, podemos calculá-lo! certo? logo:
  //RPM = (((float)D4 * 256.0) + float(D3)) / 4.0;
  //VELO = (float)D4;
  //CONS = (256*D4 + D3)/20;
  //TESTE = (100/255) * D4;
  // E com ela calculada, podemos printar na janela serial a informação útil e decodificada
  //Serial.print("Porcentagem etanol = ");
  //Serial.println(TESTE);
  
  Serial.println(D4);
  Serial.println(D3);
  Serial.println(D2);
  Serial.println(D1);
  //Serial.println(D0);
  
  // a cada 1 segundo!
  delay(2000);
}
