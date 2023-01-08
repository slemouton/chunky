#define NVELITEMS 3

struct velItem{
  char name[MAXCHAR];
  int lowvel;
  int highvel;};

struct velItem velocitytable[]= {
    "_ff",81,127,
    "_mf",61,80,
    "_pp",0,60};
