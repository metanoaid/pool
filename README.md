Требования:

Дано:
=====

Участвующие устройства:
-----------------------
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

Планируемая логика:
-------------------

1. Санузел.

При нажатии на подача/отвод воды санузла, включить вентилятор на 5 минут

2. Кнопка бассейна 2

**Положение по умолчанию. Подача воды (горячая / холодная) выключена.**

При одинарном краткосрочном нажатии на кнопку, включается подача воды.
Если вода включена, то при одинарном краткосрочном нажатии на кнопку, выключается подача воды.

3. Кнопка бассейна 1

В бассейне есть подача воды и спуск воды. Каждое из устройств может быть в состоянии открыто, закрыто. Всего может быть 4 состояния:

Подача воды | Спуск воды
--- | ---
`закрыт` | `открыт`
`открыт` | `закрыт`
`открыт` | `открыт`
`закрыт` | `закрыт`

**Состояние по умолчанию: подача воды закрыта, спуск воды открыт.**



