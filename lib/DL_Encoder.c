#include "DL_Encoder.h"

void DL_encoderTick(DL_Encoder *encoder)	//Обработчик событий энкодера
{
	static uint32_t timer_frot = 0;				//Программный таймер для событий быстрых поворотов
	static int16_t counter_frot = 0;			//Счетчик переключений при быстром повороте

	static uint32_t timer_dbc = 0;				//Программный таймер для события двойного клика

	static uint32_t timer_hold = 0;				//Программный таймер для события удержания кнопки
	static uint8_t hold_flag = 0;				//Флаг для события удержания кнопки

	uint8_t clk = HAL_GPIO_ReadPin(encoder->port, encoder->pin_clk);	//Чтение состояния на пине clk
	uint8_t data = HAL_GPIO_ReadPin(encoder->port, encoder->pin_data);	//Чтение состояния на пине data
	uint8_t sw = HAL_GPIO_ReadPin(encoder->port, encoder->pin_sw);		//Чтение состояния на пине sw

	if (clk != encoder->last_clk_state)				//Если произошёл поворот ручки энкодера
	{
		encoder->last_clk_state = clk;					//Обновить предыдущее состояние

		if (clk == data)								//Если поворот был вправо
		{
			if(encoder->handler != NULL)
				encoder->handler(RIGHT);						//Вызов обработчика с событием поворота вправо

			if(HAL_GetTick() - timer_frot < 10)				//Если с предыдущего обнуления таймера быстрого поворота прошло меньше 10мс
			{
				counter_frot++;									//Инкремент счетчика быстрого поворота

				if(counter_frot == 5)							//Проверка нужного количества быстрых поворотов вправо
					if(encoder->handler != NULL)
						encoder->handler(FASTRIGHT);					//Вызов обработчика быстрого поворота вправо
			}
			else											//Иначе, если с предыдущего обнуления таймера быстрого поворота прошло больше 10мс
				counter_frot = 0;								//Сброс счетчика быстрых поворотов
		}
		else											//Иначе, если поворот был влево
		{
			if(encoder->handler != NULL)
				encoder->handler(LEFT);							//Вызов обработчика с событием поворота влево

			if(HAL_GetTick() - timer_frot < 10)				//Если с предыдущего обнуления таймера быстрого поворота прошло меньше 10мс
			{
				counter_frot--;									//Декремент счетчика быстрого поворота

				if(counter_frot == -5)							//Проверка нужного количества быстрых поворотов влево
					if(encoder->handler != NULL)
						encoder->handler(FASTLEFT);						//Вызов обработчика быстрого поворота влево
			}
			else											//Иначе, если с предыдущего обнуления таймера быстрого поворота прошло больше 10мс
				counter_frot = 0;								//Сброс счетчика быстрых поворотов
		}

		timer_frot = HAL_GetTick();
	}

	if (sw != encoder->last_sw_state)				//Если изменилось состояние кнопки
	{
		encoder->last_sw_state = sw;					//Обновление предыдущего состояния

		if (!sw)										//Если кнопка была нажата
		{
			if(encoder->handler != NULL)
				encoder->handler(PRESS);						//Вызов обработчика с событием нажатия кнопки

			if (HAL_GetTick() - timer_dbc < 250)			//Если с предыдущего клика прошло меньше 250мс
				if(encoder->handler != NULL)
					encoder->handler(DOUBLECLICK);					//Вызов обработчика с событием быстрого двойного нажатия

			timer_dbc = HAL_GetTick();						//Сброс счетчика двойного нажатия

			timer_hold = HAL_GetTick();						//Сброс счетчика удержания кнопки
			hold_flag = 1;									//Поднять флаг разрешения обработки удержания
		}
		else											//Иначе, если кнопка была отпущена
		{
			if(encoder->handler != NULL)
				encoder->handler(RELEASE);						//Вызов обработчика с событием отпускания кнопки
			hold_flag = 0;									//Сбросить флаг разрешения обработки удержания
		}
	}

	if(HAL_GetTick() - timer_hold > 500 && hold_flag)	//Если флаг разрешения обработки удержания был поднят в течении 500мс
	{
		if(encoder->handler != NULL)
			encoder->handler(HOLD);								//Вызов обработчика с событием удержания кнопки
		hold_flag = 0;										//Сброс флага разрешения обработки удержания
	}
}
void DL_encoderInit(DL_Encoder *encoder, GPIO_TypeDef *port, uint16_t pin_clk, uint16_t pin_data, uint16_t pin_sw)
{
	encoder->port = port;

	encoder->pin_clk = pin_clk;
	encoder->pin_data = pin_data;
	encoder->pin_sw = pin_sw;

	encoder->last_clk_state = HAL_GPIO_ReadPin(port, pin_clk);
	encoder->last_sw_state = HAL_GPIO_ReadPin(port, pin_sw);

	encoder->handler = NULL;
}
void DL_encoderSetHandler(DL_Encoder *encoder, void (*handler)(DL_EncEvent))
{
	encoder->handler = handler;
}
