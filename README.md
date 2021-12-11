Требования
==========

Самое важное требование пользователей к системе: температура подачи воды в бассейне и в душе. А значит надо понять каким образом управлять электронным краном на открытие и закрытие (полное/частичное/неполное).

NB: Проверить качество коммутации и работоспособность кнопок, соединений

Устройства

==========

1. Управляющие: кнопка бассейна 1
2. Управляющие: кнопка бассейна 2
3. Управляющие: кнопка санузла
4. Управляющие: выключатель света
5. Свет (лампы)
6. Вентилятор бытовой
7. Душевина
8. Поступление воды в бассейн
9. Отвод воды из бассейна
10. Подача/отвод воды санузла
11. Кран подачи горячей воды
12. Кран подачи холодной воды

Планируемая логика

==================

1. Санузел
  
-----------

При нажатии на подача/отвод воды санузла, включить вентилятор на 5 минут и включить слив бачка.

1. Душ

-----------

*Положение по умолчанию. Подача воды (горячая / холодная) выключена.*

* Одинарное краткосрочное нажатие. (1 раз в течение 2-х секунд) - включается подача воды душа
* Двойное краткосрочное нажатие. (2 раза в течение 2-х секунд) - выключается подача воды душа

1. Бассейн

-----------

В бассейне есть устройства подачи воды (горячий и холодный) и спуск воды. Каждое из устройств может быть в состоянии открыто, закрыто. Всего может быть 4 состояния:

Подача воды | Спуск воды | Состояние
--- | --- | ---
`закрыт` | `открыт` | состояние 1
`открыт` | `закрыт` | состояние 2
`открыт` | `открыт` | состояние 3
`закрыт` | `закрыт` | состояние 4

Оперируя одной кнопкой необходимо иметь возможноть переводить подачу воды и спуск воды в одно из 4-х возможных состояний.

* *Состояние 1 по умолчанию: подача воды закрыта, спуск воды открыт.* Бассейн пуст
* Состояние 4 - используется, когда бассейн наполнен. Закрыта подача воды, закрыт слив воды.
* Состояние 2 - наполнение бассейна. Открыта подача воды, закрыт слив воды.
* Состояние 3 - замена воды в бассейне с остывшей на горячую.

Поведение кнопки
----------------

* Одинарное краткосрочное нажатие. КЛИК. (1 раз в течение 2-х секунд). Всегда переводит систему в состояние 2.
* Двойное краткосрочное нажатие. ДАБЛКЛИК. (2 раза в течение 2-х секунд). Всегда переводит систему в состояние 4.
* Долгое нажатие. (Нажать и держать в течение 3-х секунд). Всегда переводит систему в состояние 1.
* Тройное нажатие (3 раза в течение 2-х секунд) - Всегда переводит систему в состояние 3.

Упрощенное описание поведения кнопки
------------------------------------

* Один раз нажал - наполняется
* Два раза нажал - закрылся
* Три раза нажал - подогревается
* Нажал долго - спускается

![alt-состояния бассейна][pool]
 
Фотографии
-----------------

[pool]: https://github.com/metanoaid/pool/blob/main/pool_simple.png "Упрощенное состояние бассейна"
