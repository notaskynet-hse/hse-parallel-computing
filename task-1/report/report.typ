#import "gost/lib.typ": gost-work

#show: gost-work.with(
  organization: [Национальный исследовательский университет \ «Высшая школа экономики»],
  department: [Факультет прикладной математики],
  work-name: [Отчёт по заданию №1 «Освоение векторизации»],
  subject: [Параллельные вычисления],
  student-text: [Работу выполнил \ студент группы],
  student-group: [СКБ-221],
  student-name: [Спиридонов Д.],
  teacher-text: [Работу проверил],
  teacher-name: [Байдин Г.С.],
  city: [Москва],
)

#show raw.where(block: false): set text(size: 12pt)
#show raw.where(block: true): set text(size: 8pt)
#show raw.where(block: true): set par(leading: 0.5em, justify: false)
#show figure.where(kind: table): set block(breakable: true)

#let small(body) = {
  set text(size: 11pt)
  set par(leading: 0.65em, justify: false, first-line-indent: 0pt)
  show raw: set text(size: 10pt)
  body
}

#let flags(s) = s.split(" ").map(raw).join(linebreak())

#let rows = csv("/build/results.csv").slice(1)
#let time = rows.map(r => (r.at(0), float(r.at(1)))).to-dict()
#let fmt(x) = {
  let r = str(calc.round(x, digits: 2))
  let parts = r.split(".")
  if parts.len() == 1 { r + ".00" } else { r + "0" * (2 - parts.at(1).len()) }
}
#let sp(a, b) = fmt(time.at(a) / time.at(b))

= Постановка задачи

Вычислить $pi$ и сравнить время работы вариантов: без оптимизации, с ключами компилятора,
с полуавтоматической векторизацией и с ручной векторизацией на intrinsics (AVX2, AVX-512).

Используется $"Si"(x) = integral_0^x (sin t)/t d t arrow.r pi/2$ при $x arrow.r infinity$.
Интеграл на $[0, X]$ считается методом средних прямоугольников, хвост --- асимптотикой
$"Si"(X) approx pi/2 - (cos X)/X - (sin X)/X^2$:

$ pi approx 2 (h sum_(i=0)^(N-1) (sin t_i)/t_i + (cos X)/X + (sin X)/X^2), quad t_i = (i + 1/2) h, quad h = X/N. $

Параметры: $X = 10^4$, $N = 2^27$, тип `float`.

Узлы разбиты на блоки по 4096. Для блока с началом $t_0$ (в `double`)
$sin(t_0 + u) = sin t_0 cos u + cos t_0 sin u$, где $u = (j + 1/2) h <= 0.5$, поэтому $sin u$ и $cos u$
считаются полиномами Тейлора без редукции аргумента. Сумма блока накапливается во `float`,
общая сумма --- в `double`.

= Вычислительная система

#small(raw(read("/build/sysinfo.txt"), block: true))

= Реализации

#figure(
  caption: [Варианты программы],
  small(table(
    columns: (auto, auto, 1fr),
    align: left,
    table.header[*Этап*][*Ключи*][*Описание*],
    `O0`, flags("-O0"), [`pi_naive.c`: скалярный цикл с `sinf`/`cosf` и делением],
    `O3`, flags("-O3 -march=native"), [тот же код; не векторизуется: без `-ffast-math` нельзя переставлять сложения в редукции],
    `fast`, flags("-O3 -march=native -ffast-math"), [тот же код; компилятор векторизует цикл по `zmm` с вызовами `_ZGVeN16v_sinf`/`_ZGVeN16v_cosf` из libmvec],
    `semi`, flags("-O3 -march=native -ffast-math -fopenmp-simd"), [`pi_semi.c`: цикл помечен `#pragma omp simd reduction(+:s)`, векторизация задаётся явно],
    `avx2`, flags("-O3 -mavx2 -mfma"), [`pi_avx2.c`: 8 `float` в `ymm`, $sin u$ и $cos u$ полиномами через FMA, деление `vdivps`],
    `avx512`, flags("-O3"), [`pi_avx512.c`: 16 `float` в `zmm`, 4 независимых аккумулятора],
  )),
)

Массивов в программе нет. В ручных версиях индексы и почти все константы держатся в регистрах,
из памяти читаются только несколько констант полинома.

= Результаты

#let desc = (
  O0: "без оптимизации",
  O3: "ключи компилятора",
  fast: "ключи + fast-math",
  semi: "omp simd",
  avx2: "intrinsics AVX2",
  avx512: "intrinsics AVX-512",
)

#figure(
  caption: [Время (медиана 5 запусков) и ускорение],
  small(table(
    columns: (auto, 1fr, auto, auto, auto, auto),
    align: (left, left, right, right, right, right),
    table.header[*Этап*][*Вариант*][*Время, с*][*vs O0*][*vs O3*][*Ошибка*],
    ..rows
      .map(r => (
        raw(r.at(0)),
        desc.at(r.at(0)),
        r.at(1),
        sp("O0", r.at(0)),
        sp("O3", r.at(0)),
        r.at(3),
      ))
      .flatten(),
  )),
)

Ключи `-O3 -march=native` ускоряют исходную программу в #sp("O0", "O3") раза: время уходит
в скалярные `sinf`/`cosf`. Добавление `-ffast-math` разрешает векторизацию редукции,
и компилятор использует векторные `sinf`/`cosf` из libmvec --- #sp("O3", "fast")× к `O3`.
Директива `omp simd` даёт тот же код и то же время, но не зависит от эвристик автовекторизатора.

Ручная версия на AVX2 быстрее `O3` в #sp("O3", "avx2") раза, что больше теоретических 8×:
кроме векторизации, библиотечные функции заменены короткими полиномами без редукции аргумента.
Относительно `fast` выигрыш #sp("fast", "avx2")×.

Переход на AVX-512 даёт ещё #sp("avx2", "avx512")× при теоретических 2×. Zen 4 исполняет
512-битные операции как две 256-битные, поэтому число элементов, обрабатываемых за такт,
почти не растёт.

Ошибка всех вариантов не превышает $4 dot 10^(-7)$.

= Листинг ассемблера

Функция `block_avx2`, основной цикл --- метка `.L4`:

#raw(read("/build/avx2.txt"), lang: "asm", block: true)

Функция `block_semi`, вызовы векторных `sinf`/`cosf`:

#raw(read("/build/semi.txt"), lang: "asm", block: true)

= Вывод

Быстрее всех `avx512`: #fmt(time.at("avx512")) с, в #sp("O0", "avx512") раза быстрее исходной
программы. Ключи компилятора дают #sp("O0", "O3")×, `-ffast-math` и `omp simd` --- ещё
#sp("O3", "fast")× за счёт векторных `sinf`/`cosf`. Основной выигрыш даёт ручная векторизация
вместе с заменой функций на полиномы. AVX-512 на Zen 4 добавляет немного из-за 256-битных
исполнительных блоков.

= Листинг программы

#for f in ("common.h", "pi_naive.c", "pi_semi.c", "pi_avx2.c", "pi_avx512.c") [
  == #raw(f)
  #raw(read("/src/" + f), lang: "c", block: true)
]
