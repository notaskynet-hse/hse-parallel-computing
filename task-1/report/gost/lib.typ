#import "src/components.typ": titlepage
#import "src/models.typ": signatures-data, titlepage-header-data, titlepage-title-data, titlepage-data

#let gost-work(
  organization: [Название организации],
  department: [Название кафедры],
  work-name: [Вид работы],
  subject: [Название дисциплины],
  student-text: [Работу выполнил \ студент группы],
  student-group: [ГРУ-ПА],
  student-name: [Фамилия И.О.],
  teacher-text: [Работу проверил],
  teacher-name: [Фамилия И.О.],
  city: [Город],
  year: none,
  bibliography-file: "",
  body,
) = [
  #show outline.entry.where(
    level: 1
  ): it => {
    v(12pt, weak: true)
    strong(it)
  }
  #set list(marker: [--])
  #set text(lang: "ru", size: 14pt, hyphenate: true)
  #set page(
    paper: "a4",
    margin: (
      top: 2cm,
      bottom: 2cm,
      right: 1cm,
      left: 3cm
    )
  )

  #titlepage(
    titlepage-data(
      titlepage-header-data(organization, department),
      titlepage-title-data(work-name, subject),
      signatures-data(
        student-text,
        student-group,
        student-name,
        teacher-text,
        teacher-name
      )
    ),
    city,
    year
  )

  #set heading(numbering: "1.")
  #set page(numbering: "1")

  #outline(
    title: [Содержание],
    depth: 3,
  )

  #set par(
    justify: true,
    first-line-indent: 1.25cm,
    leading: 1.5em,
  )

  #pagebreak()

  #body

  #if bibliography-file != "" [
    = Список литературы
    #bibliography(title: none, bibliography-file)
  ]
]