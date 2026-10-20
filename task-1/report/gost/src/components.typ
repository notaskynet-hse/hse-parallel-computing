#import "models.typ": *

#let titlepage-header(data) = [
  #smallcaps[*Правительство Российской Федерации*]
  #line(length: 95%, stroke: 0.5pt)
  #smallcaps(data.organization)
  #v(1em)
  #data.department
]

#let titlepage-title(data) = [
  #strong[
    #text(1em + 2pt)[#data.work-name] \
    по дисциплине \
    #text(1em + 4pt)["#data.subject"]
  ]
]

#let _sign-line(width: auto, height: auto, body) = block(width: width, height: height)[
  #place(bottom)[#line(length: 100%, stroke: 0.5pt)]
  #place(bottom + center, dy: 1em)[#body]
]

#let signatures(data) = [
  #set grid(columns: 3, align: bottom, column-gutter: 3em, row-gutter: 3em)
  #show grid.cell.where(x: 0): set align(left)
  #show grid.cell.where(x: 2): set align(left)
  #set text(size: 1em - 2pt)
  #grid(
    [#data.student-text #data.student-group],
    _sign-line(width: 15em, height: 2em)[подпись, дата],
    data.student-name,
    data.teacher-text,
    _sign-line(width: 15em, height: 1em)[подпись, дата],
    data.teacher-name
  )
]

#let titlepage(data, city, year) = [
  #set align(center)
  #set text(size: 14pt)
  #titlepage-header(data.header-data)
  #v(9em)
  #titlepage-title(data.title-data)
  #v(4em)
  #signatures(data.signatures-data)

  #v(1fr)

  #city #if year != none [#year] else [#datetime.today().year()]

  #pagebreak()
]