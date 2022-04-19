#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// Déclaration de la structure
struct valeurCapteurs {
    int analogique;
    int numerique;
    double tempsEnMillisecondes;
};

// Déclaration du semaphore
SemaphoreHandle_t xSerialSemaphore = NULL;

// Déclaration des ports
int pot = A0;
int but1 = 2;
int but2 = 3;

// Déclaration des queues
QueueHandle_t qpot;
QueueHandle_t qbut;
QueueHandle_t qstruct;
QueueHandle_t qsensor;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  while (!Serial);

  if ( xSerialSemaphore == NULL )  
  {
    xSerialSemaphore = xSemaphoreCreateMutex();
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );
  }

  // Initialisation des ports
  pinMode(pot, INPUT);
  pinMode(but1, INPUT);
  pinMode(but2, INPUT);

  // Initialisation des queues
  qpot = xQueueCreate(4, sizeof(int));
  qbut = xQueueCreate(4, sizeof(int));
  qstruct = xQueueCreate(4, sizeof(valeurCapteurs));
  qsensor = xQueueCreate(4, sizeof(valeurCapteurs));

  // Initialisation des tâches
  xTaskCreate(task1, "Lis la valeur du potentiomètre", 100, NULL, 0, NULL);
  xTaskCreate(task2, "Lis la valeur des boutons", 100, NULL, 0, NULL);
  xTaskCreate(task3, "Initialise la structure", 100, NULL, 0, NULL);
  xTaskCreate(task4, "Affiche la structure", 100, NULL, 0, NULL);
  xTaskCreate(task5, "Affiche la nouvelle structure", 100, NULL, 0, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void task1(void *pvparameters){
  int datapot;
  while(1){
    datapot = analogRead(pot);
    xQueueSend(qpot, &datapot, 0);

    vTaskDelay(100);
  }
}

void task2(void *pvparameters){
  int databuttons;
  while(1){
    databuttons = digitalRead(but1) + digitalRead(but2);
    xQueueSend(qbut, &databuttons, 0);

    vTaskDelay(100);
  }
}

void task3(void *pvparameters){
  int analog;
  int num;
  valeurCapteurs s;
  while(1){
    xQueueReceive(qpot, &analog, 0);
    xQueueReceive(qbut, &num, 0);

    s.analogique = analog;
    s.numerique = num;
    s.tempsEnMillisecondes = millis();

    xQueueSend(qstruct, &s, 0);

    vTaskDelay(100);
  }
}

void task4(void *pvparameters){
  valeurCapteurs s;
  while(1){
    xQueueReceive(qstruct, &s, 0);
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.print("1 Analog : ");
      Serial.print(s.analogique);
      Serial.print("    Num : ");
      Serial.print(s.numerique);
      Serial.print("    Millis : ");
      Serial.print(s.tempsEnMillisecondes);
      Serial.print("\n");
      xSemaphoreGive( xSerialSemaphore );
    }
    xQueueSend(qsensor, &s, 0);

    vTaskDelay(100);
  }
}

void task5(void *pvparameters){
  valeurCapteurs s;
  valeurCapteurs news;
  while(1){
    xQueueReceive(qsensor, &s, 0);
    news.analogique = s.analogique;
    news.numerique = s.numerique;
    news.tempsEnMillisecondes = s.tempsEnMillisecondes/60000;
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.print("2 Analog : ");
      Serial.print(news.analogique);
      Serial.print("    Num : ");
      Serial.print(news.numerique);
      Serial.print("    Millis : ");
      Serial.print(news.tempsEnMillisecondes);
      Serial.print("\n");
      xSemaphoreGive( xSerialSemaphore );
    }

    vTaskDelay(100);
  }
}