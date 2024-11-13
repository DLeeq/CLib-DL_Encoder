#ifndef DL_ENCODER_H
#define DL_ENCODER_H

#include "stm32f1xx_hal.h"	//Подключение HAL библиотеки, соответствующей вашему МК

typedef enum {LEFT, RIGHT, FASTLEFT, FASTRIGHT, PRESS, RELEASE, DOUBLECLICK, HOLD} DL_EncEvent;	//Перечисление событий для обработчика

typedef struct								//Структура с данными энкодера
{
	GPIO_TypeDef *port;							//На каком порту подключен

	uint16_t pin_clk;							//Пин подключения clk
	uint16_t pin_data;							//Пин подключения data
	uint16_t pin_sw;							//Пин подключения кнопки

	uint8_t last_clk_state;						//Последнее состояние пина clk
	uint8_t last_sw_state;						//Последнее состояние пина кнопки

	void (*handler)(DL_EncEvent);				//Обработчик событий энкодера

	int32_t pos;								//Позиция энкодера
} DL_Encoder;

void DL_encInit(DL_Encoder *encoder, GPIO_TypeDef *port, uint16_t pin_clk, uint16_t pin_data, uint16_t pin_sw);  //Инициализатор
void DL_encSetHandler(DL_Encoder *encoder, void (*handler)(DL_EncEvent));  //Привязка обработчика событий энкодера

void DL_encTick(DL_Encoder *encoder);  //Функция "Тикер" для программного отслеживания событий

uint32_t DL_encGetPos(DL_Encoder *encoder);  //Возврат позиции энкодера
void DL_encSetPos(DL_Encoder *encoder, uint32_t pos);  //Установка позиции энкодера
void DL_encSetNull(DL_Encoder *encoder);  //Установка нулевой позиции энкодера
#endif
