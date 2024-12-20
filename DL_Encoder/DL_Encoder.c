#include "DL_Encoder.h"

void DL_encInit(DL_Encoder *encoder, GPIO_TypeDef *port, uint16_t pin_clk, uint16_t pin_data, uint16_t pin_sw)  //Инициализатор
{
	encoder->port = port;  //Порт подключения энкодера

	encoder->pin_clk = pin_clk;  //Пин подключения clk
	encoder->pin_data = pin_data;  //Пин подключения data
	encoder->pin_sw = pin_sw;  //Пин подключения кнопки

	encoder->last_clk_state = HAL_GPIO_ReadPin(port, pin_clk);  //Предыдущее состояние пина clk
	encoder->last_sw_state = HAL_GPIO_ReadPin(port, pin_sw);  //Предыдущее состояние пина кнопки

	encoder->handler = NULL;  //Обработчик не задан

	encoder->timer_frot = 0;  //Программный таймер для событий быстрых поворотов
	encoder->counter_frot = 0;  //Счетчик переключений при быстром повороте

	encoder->timer_dbc = 0;  //Программный таймер для события двойного клика

	encoder->timer_hold = 0;  //Программный таймер для события удержания кнопки
	encoder->hold_flag = 0;  //Флаг для события удержания кнопки

	encoder->pos = 0;  //Счетчик позиции равен нулю
}
void DL_encSetHandler(DL_Encoder *encoder, void (*handler)(DL_EncEvent))  //Привязка обработчика событий энкодера
{
	encoder->handler = handler;
}

void DL_encTick(DL_Encoder *encoder)  //Тикер для генерации событий и обновления счетчика поворотов
{
	uint8_t clk = HAL_GPIO_ReadPin(encoder->port, encoder->pin_clk);	//Чтение состояния на пине clk
	uint8_t data = HAL_GPIO_ReadPin(encoder->port, encoder->pin_data);	//Чтение состояния на пине data
	uint8_t sw = HAL_GPIO_ReadPin(encoder->port, encoder->pin_sw);		//Чтение состояния на пине sw

	if (clk != encoder->last_clk_state)				//Если произошёл поворот ручки энкодера
	{
		encoder->last_clk_state = clk;					//Обновить предыдущее состояние

		if (clk != data)								//Если поворот был вправо
		{
			if(encoder->handler != NULL)					//Если обработчик задан
				encoder->handler(DL_ENC_RIGHT);					//Вызов обработчика с событием поворота вправо
			encoder->pos++;									//Инкремент позиции

			if(HAL_GetTick() - encoder->timer_frot < 10)		//Если с предыдущего обнуления таймера быстрого поворота прошло меньше 10мс
			{
				encoder->counter_frot++;							//Инкремент счетчика быстрого поворота

				if(encoder->counter_frot == 5)					//Проверка нужного количества быстрых поворотов вправо
					if(encoder->handler != NULL)					//Если обработчик задан
						encoder->handler(DL_ENC_FASTRIGHT);				//Вызов обработчика быстрого поворота вправо
			}
			else											//Иначе, если с предыдущего обнуления таймера быстрого поворота прошло больше 10мс
				encoder->counter_frot = 0;						//Сброс счетчика быстрых поворотов
		}
		else											//Иначе, если поворот был влево
		{
			if(encoder->handler != NULL)					//Если обработчик задан
				encoder->handler(DL_ENC_LEFT);						//Вызов обработчика с событием поворота влево
			encoder->pos--;									//Декремент позиции

			if(HAL_GetTick() - encoder->timer_frot < 10)	//Если с предыдущего обнуления таймера быстрого поворота прошло меньше 10мс
			{
				encoder->counter_frot--;						//Декремент счетчика быстрого поворота

				if(encoder->counter_frot == -5)					//Проверка нужного количества быстрых поворотов влево
					if(encoder->handler != NULL)					//Если обработчик задан
						encoder->handler(DL_ENC_FASTLEFT);				//Вызов обработчика быстрого поворота влево
			}
			else											//Иначе, если с предыдущего обнуления таймера быстрого поворота прошло больше 10мс
				encoder->counter_frot = 0;						//Сброс счетчика быстрых поворотов
		}

		encoder->timer_frot = HAL_GetTick();
	}

	if (sw != encoder->last_sw_state)				//Если изменилось состояние кнопки
	{
		encoder->last_sw_state = sw;					//Обновление предыдущего состояния

		if (!sw)										//Если кнопка была нажата
		{
			if(encoder->handler != NULL)					//Если обработчик задан
			{
				encoder->handler(DL_ENC_PRESS);					//Вызов обработчика с событием нажатия кнопки
				encoder->handler(DL_ENC_TOGGLE);				//Вызов обработчика с событием переключения кнопки
			}

			if (HAL_GetTick() - encoder->timer_dbc < 250)	//Если с предыдущего клика прошло меньше 250мс
				if(encoder->handler != NULL)					//Если обработчик задан
					encoder->handler(DL_ENC_DOUBLECLICK);			//Вызов обработчика с событием быстрого двойного нажатия

			encoder->timer_dbc = HAL_GetTick();				//Сброс счетчика двойного нажатия

			encoder->timer_hold = HAL_GetTick();			//Сброс счетчика удержания кнопки
			encoder->hold_flag = 1;							//Поднять флаг разрешения обработки удержания
		}
		else											//Иначе, если кнопка была отпущена
		{
			if(encoder->handler != NULL)					//Если обработчик задан
			{
				encoder->handler(DL_ENC_RELEASE);				//Вызов обработчика с событием отпускания кнопки
				encoder->handler(DL_ENC_TOGGLE);				//Вызов обработчика с событием переключения кнопки
			}
			encoder->hold_flag = 0;							//Сбросить флаг разрешения обработки удержания
		}
	}

	if(HAL_GetTick() - encoder->timer_hold > 500 && encoder->hold_flag)	//Если флаг разрешения обработки удержания был поднят в течении 500мс
	{
		if(encoder->handler != NULL)						//Если обработчик задан
			encoder->handler(DL_ENC_HOLD);						//Вызов обработчика с событием удержания кнопки
		encoder->hold_flag = 0;								//Сброс флага разрешения обработки удержания
	}
}

uint32_t DL_encGetPos(DL_Encoder *encoder)  //Вернуть позицию энкодера
{
	return encoder->pos;
}
void DL_encSetPos(DL_Encoder *encoder, uint32_t pos)  //Задать позицию энкодера
{
	encoder->pos = pos;
}
void DL_encSetNull(DL_Encoder *encoder)  //Установка нулевой позиции
{
	encoder->pos = 0;
}
