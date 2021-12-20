//
//  Copyright DtS 12.12.2021
// 
//  Описание лагаритма читаем в readme.txt
// 
//
#include <Arduino.h>
#include "src\Consts.h"
#include "src\DEF_Message.h"
#include "src\Messages.h"
#include "src\SoftTimers.h"
#include "src\TLed.h"   
#include "src\DigitalButton.h"

// 
//  описание используемых пинов
//
constexpr uint8_t PIN_LED_ALIVE		= 13;	// светодиод активности, мигает раз в пять секунд
constexpr uint8_t PIN_DRAIN_RELAY	= 12;	// реле включения смыва
constexpr uint8_t PIN_COOLER		  = 11;	// реле вентилятора туалета

constexpr uint8_t PIN_DRAIN_BUTTON	= 2;	// кнопка смыва унитаза


//
//  константы
//
constexpr uint32_t ALIVE_PERIOD = 5000;  // период мигания светодиода активности 5 секунд
constexpr uint32_t ALIVE_ON_TIME = 150;  // 150 миллисекунд горит
constexpr uint32_t ALIVE_OFF_TIME = ALIVE_PERIOD - ALIVE_ON_TIME; // остальное время - не горит

constexpr uint8_t key_Drain = 0x10;  // код кнопки смыва

//
//  
//
TMessageList MessageList(10);	// Очередь глубиной 10 сообщений
TSoftTimers  Timers(8);			// Список независимых таймеров, 8 штук

//
//	Блок светодиода активности
// 
//  Когда всё в порядке, коротко мигает раз в 5 секунд
//  При ошибке  светится постоянно
//
TLed ledAlive(PIN_LED_ALIVE, ACTIVE_HIGH);  //  светодиод
THandle hAliveTimer = Timers.Add(ALIVE_ON_TIME, TTimerState::Stopped); // и его таймер

//
//	Блок управления туалетом, обьявления
// 
//	кнопка	для смыва			активный уровень низкий, вкл - замыканием на GND
//  реле	смыва				активный уровень низкий
//  реле    вентилятор вытяжки	активный уровень низкий
//  таймер  вентилятора вытяжки
//

enum class TToiletState : uint8_t { Unknown = 0, Normal, DrainOn, Fill, Cooler, Error };

constexpr uint32_t COOLER_TIME	= 5UL * 60 * 1000;	// таймаут вентилятора вытяжки 5 минут	default
constexpr uint32_t DRAIN_TIME	= 5UL * 1000;		// таймаут реле смыва, 5 секунд			default
constexpr uint32_t FILL_TIME	= 1UL * 60 * 1000;	// время наполнения бачка, 1 минута		default

TDigitalButton btnDrain(PIN_DRAIN_BUTTON, key_Drain, ACTIVE_LOW);	// кнопка смыва
TDigitalDevice relayDrain(PIN_DRAIN_RELAY, ACTIVE_LOW);				// реле смыва 
TDigitalDevice relayCooler(PIN_COOLER, ACTIVE_LOW);					// реле вентилятора вытяжки

THandle hToiletTimer = Timers.Add(DRAIN_TIME, TTimerState::Stopped); // таймер туалета, многоцелевой
THandle hCoolerTimer = Timers.Add(COOLER_TIME, TTimerState::Stopped);// таймер вентилятора вытяжки

TToiletState ToiletState = TToiletState::Unknown; // текущий статус сортира

void SetToiletState(const TToiletState ANewState); // прототип функции смены статуса туалета

void OnDrainButtonPressed(void); // функция нажатия кнопки смыва
//
// ---------  Конец обьявлений туалетного блока
//


//   ------------------------------------------------------------------
// 
// 
//   implementation
//
//
//   ------------------------------------------------------------------


//
//  служебная функция для поддержки работы функций ввода/вывода
//  таких как printf(...), puts(...) и т.д.
//  перенаправляет все выводимые строки в Serial
//
int srl_putchar(char ch, FILE* f) {
	return Serial.print(ch);
}


//
//  Первоначальные настройки при запуске
// 
//
void setup() {
  Serial.begin(115200);					// инициализируем сериал
  stdout = fdevopen(srl_putchar, NULL);	// перенаправляем весь вывод в него
  puts("Program MegaPool v1.0  started...");// выводим приветственное сообщение в Serial


  ledAlive.On();						// включаем светодиод активности
  Timers.Start(hAliveTimer);			// и запускаем таймер его мигания

  SetToiletState(TToiletState::Normal); // после сброса всё выключено
}

//
//  функция  смены состояния туалета
//
void SetToiletState(const TToiletState ANewState) {
	
	if (ToiletState == ANewState) return;  // если состояние не поменялось - делать ничего не надо

	ToiletState = ANewState;

	switch (ToiletState)
	{
	case TToiletState::Unknown:
		SetToiletState(TToiletState::Error); // если входим в это состояние - это ошибкаж
		break;
	
	case TToiletState::Normal: // в нормальном состоянии всё выключено и все таймеры остановлены
		relayDrain.Off();
		relayCooler.Off();
		Timers.Stop(hCoolerTimer);
		Timers.Stop(hToiletTimer);
		break;

	case TToiletState::DrainOn:		// после нажатия на кнопку смыва попадаем сюда
		relayCooler.On();			// включаем сразу вентилятор
		Timers.Reset(hCoolerTimer);	// настраиваем его таймер (выключится через СOOLER_TIME миллисекунд)
		relayDrain.On();			// включаем реле управления смывом
		Timers.SetNewInterval(hToiletTimer, DRAIN_TIME); // настрайваем его выключение (через DRAIN_TIME миллсекунд)
		break;

	case TToiletState::Fill:	// ждём наполнения бачка
		relayDrain.Off();		// выключаем реле устройства смыва
		Timers.SetNewInterval(hToiletTimer, FILL_TIME); 
		Timers.Reset(hToiletTimer);		// настраиваем таймер на ожидание наполнения бачка (FILL_TIME миллисекунд)
		break;

	case TToiletState::Error:		// состояние ошибки
		Timers.Stop(hCoolerTimer);	// останавливаем все таймеры
		Timers.Stop(hToiletTimer);	
		Timers.Stop(hAliveTimer);

		relayCooler.Off();			// выключаем все реле
		relayDrain.Off();
		ledAlive.On();				// светодиод активности теперь горит не мигая

		break;

	default:
		break;
	}

}


//
//  нажата кнопка смыва
//
void OnDrainButtonPressed(void)
{
	if ((ToiletState == TToiletState::Normal) || (ToiletState == TToiletState::Cooler)) {

		SetToiletState(TToiletState::DrainOn); // смыв

	}
}



//
//  главная функция обработки всех сообщений программы, 
//  сюда стекаются все сообщения от всех сенсоров и устройств
//
void Dispatch(const TMessage& Msg) {
	switch (Msg.Message)
	{
	case msg_TimerEnd:

		if (Msg.LoParam == hAliveTimer) {  // сработал таймер светодиода активности
			ledAlive.Toggle();
			uint32_t newInterval = ledAlive.isOn() ? ALIVE_ON_TIME : ALIVE_OFF_TIME;
			Timers.SetNewInterval(hAliveTimer, newInterval); // меняем интервал 
		}

		if (Msg.LoParam == hCoolerTimer) {	// сработал таймер вентилятора
			SetToiletState(TToiletState::Normal); // всё выключаем
		}

		if (Msg.LoParam == hToiletTimer) { // сработал сортирный таймер
			if (ToiletState == TToiletState::Fill) SetToiletState(TToiletState::Cooler);
			if (ToiletState == TToiletState::DrainOn) SetToiletState(TToiletState::Fill);
		}

		break;

	case msg_KeyPress:
		if (Msg.LoParam == key_Drain) OnDrainButtonPressed(); // коротко нажали и отпустили кнопку смыва
		break;

	case msg_LongPress:  // длинное нажатие пока не обрабатываем
		break; 

	default:
		printf("Unhandled message %04X, param Lo = %04X, Hi = %04X\n", Msg.Message, Msg.LoParam, Msg.HiParam);
		break;
	}
}

//
//  бесконечная функция опроса всех кнопок и сенсоров
//  при изменении состояния сенсора он присылает сообщение в очередь
//  здесь оно выбирается и передается на обработку в функцию Dispatch()
//
void loop() {

	btnDrain.Read();		// чтение кнопки смыва

	Timers.Run();			// цикл перебора таймеров 

	if (MessageList.Available()) Dispatch(MessageList.GetMessage()); // если пришло сообщение - перешлём его в Dispatch()

}
