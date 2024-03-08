#include <AccelStepper.h>

//Metodo Comunicacao
#define ESCOLHA_BT true
#define ESCOLHA_SERIAL false 

//Pinos
#define PINO_MOTOR_DC1 D1
#define PINO_MOTOR_DC2 D2
#define PINO_MOTOR_A D5
#define PINO_MOTOR_B D6
#define PINO_FIM_DE_CURSO_1 D3
#define ENABLE D7
//Cria motor 
AccelStepper stepper(1, PINO_MOTOR_A, PINO_MOTOR_B);

//Constantes
const long REGIAO_ATUACAO_CARRO = 1000;

//ESTADOS DO CARRO
volatile bool MOVIMENTANDO_CARRO = false;
volatile bool LIMPANDO_PAINEL = false;

//Variaveis globais 
char comando;
String texto;
//Funcoes
String verifica_seriais();
void imprimi_informacoes();
void menu_escolha();
void movimentar_carro();
void limpar_painel();

// //Interrupcoes

  void IRAM_ATTR carro_fim()
  {
    Serial.println("interupt1");
    stepper.disableOutputs();
    LIMPANDO_PAINEL = false;
    MOVIMENTANDO_CARRO = false;
    digitalWrite(ENABLE, HIGH);
    analogWrite(D1,0);
  }

void setup()
{
  //PINOS
  
  pinMode(PINO_MOTOR_DC1,OUTPUT);
  pinMode(PINO_MOTOR_DC2,OUTPUT); 
  analogWriteFreq(1000);
  pinMode(ENABLE, OUTPUT); 
  //FIM DE CURSOS
  pinMode(PINO_FIM_DE_CURSO_1, INPUT_PULLUP);
  attachInterrupt(PINO_FIM_DE_CURSO_1,carro_fim, FALLING);


  //CONFIGURA SERIAL
  Serial.begin(9600);
 
  //CONFIGURA MOTOR PASSO
  stepper.setMaxSpeed(2000); //SPEED = Steps / second
  stepper.setAcceleration(1000); //ACCELERATION = Steps /(second)^2
  //stepper.disableOutputs();

  //AGUARDA PARA INICIAR
  delay(1000);
}
 
void loop()
{
  //analogWrite(PINO_MOTOR_DC2,1000);
  
  menu_escolha();

  //OPERACOES
  if(MOVIMENTANDO_CARRO)
    movimentar_carro();
  
  else if(LIMPANDO_PAINEL)
   limpar_painel();

}


String verifica_seriais() //testar
{
  if (Serial.available() > 0) //if something comes
  {
    return Serial.readString();
  }
  return "";
}

void imprimi_informacoes() //ajustar
{
  Serial.println(stepper.currentPosition());
}

void menu_escolha() //ajustar
{
  
 
if (Serial.available()) {
     comando = Serial.read();
  
  //MOVER CARRO
   if (comando =='m' )
  {
    texto.remove('m');
    int novaPosicao = texto.toInt();//trasforma inteiro 
    texto ="";//nada
   
    MOVIMENTANDO_CARRO = true;

    stepper.enableOutputs();
    stepper.moveTo(novaPosicao);
     Serial.println("movendo");
  }
  
  //MOVER BASE
 if (comando =='M' )
  {
    texto.remove('M');
    int novaPosicao = texto.toInt();//trasforma inteiro 
    texto ="";//nada
   
    analogWrite(PINO_MOTOR_DC1,novaPosicao);
    
     Serial.println("movendo");
  }

  //OPERACAO LIMPEZA
 else if (comando == 'l')
  {
    texto.remove('m');
    int novaPosicao = texto.toInt();//trasforma inteiro 
    texto ="";//nada
    
    LIMPANDO_PAINEL = true;
    MOVIMENTANDO_CARRO = LIMPANDO_PAINEL;

    stepper.enableOutputs();
    stepper.moveTo(novaPosicao);
     Serial.println("limpando");
  }
  else if (comando == '@')
  {
      texto ="";//nada
      Serial.println("apago");
  }
  else {
      texto.concat(comando);//concatenando os valores
    }
}
}

void movimentar_carro() //Testar
{
  if (stepper.distanceToGo() == 0)
  {
    MOVIMENTANDO_CARRO = false;
    stepper.disableOutputs();
    digitalWrite(ENABLE, HIGH);
  }
  else
  {
    digitalWrite(ENABLE, LOW);
    //Serial.println(" rum ");
    stepper.run();
  }
}

void limpar_painel() //add LIMPANDO_PAINEL = false ao chegar no endstop
{
  if(!MOVIMENTANDO_CARRO)
  {
    digitalWrite(ENABLE, HIGH);
    stepper.moveTo(-1 * stepper.currentPosition());
    MOVIMENTANDO_CARRO = true;
  }
}