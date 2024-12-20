# DL_Encoder
**Простая библиотека программной обработки инкрементального энкодера для микроконтроллеров STM32 на языке C и библиотеке HAL.**
## Оглавление
1. [Описание](#описание)
2. [Установка](#установка)
3. [Использование](#использование)
4. [Совместимость](#совместимость)
5. [Версии](#версии)
6. [Обратная связь](#обратная-связь)
## Описание  
Библиотека предлагает простой и понятный интерфейс для инициализации и программной обработки сигналов энкодера.
- Количество подключаемых энкодеров ограничена числом свободных пинов МК
- Инициализация энкодера в две строки
- Автоматический расчет позиции энкодера
- Задание нуля
- Задание позиции
- Возврат позиции энкодера
- Возможность подключить обработчик событий
- События:
  * Поворот вправо
  * Поворот влево
  * Быстрый поворот вправо
  * Быстрый поворот влево
  * Нажатие кнопки
  * Отпускание кнопки
  * Изменение состояния кнопки
  * Двойное нажатие на кнопку
  * Удержание кнопки

Библиотека снабжена подробными комментариями.  
Библиотека разработана на основе энкодера KY-040.  
Антидребезг кнопки должен быть реализован аппаратно.  
Кнопка должна подключать землю.

Для правильной работы сигнал вашего энкодера должен соответствовать следующей диаграмме:  
![Диаграмма сигналов энкодера](/images/diagram.png)  

***Важно:***  
***- Если ваш энкодер работает инвертированно относительно вращения ручки, просто поменяйте местами пины data и clk.***  
***- Пины могут называться иначе в зависимости от вашего типа энкодера***
## Установка
**Установка в директорию с пользовательскими библиотеками (Рекомендуется):**
 1. Скачайте и добавьте директорию **DL_Encoder** в папку с пользовательскими библиотеками вашего проекта (создайте папку, если её нет)
 2. Укажите пути в вашей IDE для директории с заголовочным файлом и файлом реализации
 3. Подключите библиотеку с помощью директивы include:
   ```c
   #include "DL_Encoder.h"  //Если указаны пути в IDE путь можно не прописывать
   ```
 4. Не забудьте заменить в файле **DL_Encoder.h** подключаемую HAL библиотеку на вашу версию МК
   ```c
   #include "stm32f1xx_hal.h"  //Замените на свою версию
   ```
**Установка в корень проекта (Проще):**
  1. Добавьте заголовочный файл **DL_Encoder/DL_Encoder.h** в директорию с вашими заголовочными файлами проекта
  2. Добавьте файл реализации **DL_Encoder/DL_Encoder.c** в директорию с вашими заголовочными файлами проекта
  3. Подключите библиотеку с помощью директивы include:
   ```c
   #include "DL_Encoder.h"  //Если библиотека установлена в корень вашего проекта
   ```
  4. Не забудьте заменить в файле **DL_Encoder.h** подключаемую HAL библиотеку на вашу версию МК
   ```c
   #include "stm32f1xx_hal.h"  //Замените на свою версию
   ```
***Важно:***
***Внимательно проверяйте пути к заголовочному файлу и файлу реализации***
## Использование
Правильно проинициализируйте пины на вход, с которыми работает ваш энкодер.  
Подтяжки зависят от вашего типа энкодера. 

Создание экземпляра структуры энкодера и инициализация:
```c
DL_Encoder my_enc;  //Структура энкодера
DL_encInit(&my_enc, GPIOA, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2);  //Параметры: Структура, Порт, Пин clk, Пин dt, Пин кнопки
```
В основной цикл программы добавьте функцию-тикер, передав туда вашу структуру:
```c
while (1)
{
 	DL_encTick(&my_enc);  //Функция-тикер, обязательная для работы
 	...
}
```
Для работы с позицией энкодера:
```c
DL_encGetPos(&my_enc);  //Вернуть позицию
DL_encSetPos(&my_enc, 15);  //Установить позицию, например 15
DL_encSetNull(&my_enc);  //Сбросить позицию в ноль
```
Для работы с кнопкой и расширенной работы с вращением энкодера нужно создать и привязать обработчик.  

Создание обработчика со всеми возможными событиями (вы можете оставить только те, которые вам нужны):
```c
void myEncHandler(DL_EncEvent event)
{
	switch (event)
	{
		case DL_ENC_LEFT:
			//Код, который вызывается при повороте ручки влево
			break;
		case DL_ENC_FASTLEFT:
			//Код, который вызывается при быстром повороте ручки влево
			break;
		case DL_ENC_RIGHT:
			//Код, который вызывается при повороте ручки вправо
			break;
		case DL_ENC_FASTRIGHT:
			//Код, который вызывается при быстром повороте ручки вправо
			break;
		case DL_ENC_PRESS:
			//Код, который вызывается при нажатии на кнопку
			break;
		case DL_ENC_RELEASE:
			//Код, который вызывается при отпускании кнопки
			break;
		case DL_ENC_TOGGLE:
			//Код, который вызывается при изменении состояния кнопки
			break;
		case DL_ENC_DOUBLECLICK:
			//Код, который вызывается при двойном нажатии кнопки
			break;
		case DL_ENC_HOLD:
			//Код, который вызывается при удержании кнопки
			break;
	}
}
```

Привязать обработчик:
```c
DL_encSetHandler(&my_enc, myEncHandler);  //Теперь можно обрабатывать события =)
```
## Совместимость
Библиотека написана, протестирована и гарантированно работает на отладочной плате "Blue Pill Plus" на базе МК STM32F103C8T6.
Благодаря использованию HAL, библиотека должна работать на подавляющем большинстве МК STM32.
## Версии
- v1.0
- v1.1
  - Исправлены мелкие недочёты
- v1.2
  - Исправлен потенциальный конфликт имен событий
  - Оптимизирован размер структуры энкодера
  - Исправлена ошибка таймеров и флагов
  - Добавлено событие DL_ENC_TOGGLE для кнопки
## Обратная связь
Если вы нашли баг, или есть предложения по доработке, пишите мне в [Телеграм - группу](https://t.me/DLeeFB) для обратной связи =)
