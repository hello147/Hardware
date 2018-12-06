



typedef struct
{
  int year;
  int month;
  int day;
  int hour;
  int min;
  int sec;
  unsigned int secTemp;
  char li;
  int daysInMonth;
  char date; // 0->Sun 1->Mon 2->Tue 3->Wed 4->Thur 5->Fri. 6->Sat.
}NTP;