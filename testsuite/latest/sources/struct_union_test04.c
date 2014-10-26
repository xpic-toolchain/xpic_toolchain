/* PR target/14262 */
/* { dg-do run } */

void ab (void)
{
  while (1)
  {
    ;
  }
}

typedef char   ACS;
typedef char   LSM;
typedef char   PANEL;
typedef char   DRIVE;
typedef struct {
    ACS             acs;
    LSM             lsm;
} LSMID;
typedef struct {
    LSMID           lsm_id;
    PANEL           panel;
} PANELID;
typedef struct {
    PANELID         panel_id;
    DRIVE           drive;
} DRIVEID;

void sub (DRIVEID driveid)
{
  if (driveid.drive != 1)
    ab ();
  if (driveid.panel_id.panel != 2)
    ab ();
  if (driveid.panel_id.lsm_id.lsm != 3)
    ab ();
  if (driveid.panel_id.lsm_id.acs != 4)
    ab ();
}

int main(void)
{
  DRIVEID driveid;

  driveid.drive = 1;
  driveid.panel_id.panel = 2;
  driveid.panel_id.lsm_id.lsm = 3;
  driveid.panel_id.lsm_id.acs = 4;

  sub(driveid);

  return 0;
}


