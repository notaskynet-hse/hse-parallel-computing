#let titlepage-header-data(organization, department) = (
  organization: organization,
  department: department,
)

#let titlepage-title-data(work-name, subject) = (
  work-name: work-name,
  subject: subject,
)

#let signatures-data(
  student-text,
  student-group,
  student-name,
  teacher-text,
  teacher-name,
) = (
  student-text: student-text,
  student-group: student-group,
  student-name: student-name,
  teacher-text: teacher-text,
  teacher-name: teacher-name,
)

#let titlepage-data(header-data, title-data, signatures-data) = (
  header-data: header-data,
  title-data: title-data,
  signatures-data: signatures-data,
)