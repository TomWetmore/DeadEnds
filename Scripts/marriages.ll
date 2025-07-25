
proc main ()
{
  list(mnu)
  enqueue(mnu, "List marriages by husband")
  enqueue(mnu, "List marriages by wife")
  enqueue(mnu, "List marriages by either spouse")
  enqueue(mnu, "List marriages by year")
  enqueue(mnu, "List marriages by place")
  /*set(chc, 3)*/
  set(chc, 4)
  /*set(chc, menuchoose(mnu))
  set(chc, menuchoose(mnu))
  set(chc, menuchoose(mnu))
  set(chc, menuchoose(mnu))*/
  if (eq(chc, 0)) { return(0) }

  set(ct, 0) /* count #records processed */
  set(ctx, 0) /* count module 100 for status feedback */

  set(rptinterval, 100) /* report progress every this many records */

  /* for choices 1-3, populate indiset of married individuals */
  indiset(results)

  /* for choices 4-5, populate list of families */
  list(marriages)
  list(infos)

  if (gt(chc, 3)) {
    /* Record all marriages (along with date or place) */
    forfam(fam, n) {
      enqueue(marriages, fam)
      if (eq(chc, 4)) {
        enqueue(infos, year(date(marriage(fam))))
      } else {
        enqueue(infos, place(marriage(fam)))
      }
      /* display feedback on screen */
      incr(ct)
      incr(ctx)
      if (eq(ctx, rptinterval)) {
        print(d(ct), "F ")
        set(ctx, 0)
      }
    }
  } else {
    /* Record all married persons, of appropriate gender */
    forindi(indi, n) {
      if (gt(nspouses(indi), 0)) {
        if (or(and(eq(chc, 1), male(indi)),
               and(eq(chc, 2), female(indi)),
               eq(chc, 3))) {
          addtoset(results, indi, 0)
        }
      }
      /* display feedback on screen */
      incr(ct)
      incr(ctx)
      if (eq(ctx, rptinterval)) {
        print(d(ct), "I ")
        set(ctx, 0)
      }
    }
  }
  print(nl())
  set(count, length(results))
  if (gt(chc, 3)) { set(count, length(marriages)) }
  print("Sorting ", d(count), " results")
  print(nl())
  if (gt(chc, 3)) {
    sort(marriages, infos)
  } else {
    namesort(results)   
  }
  print("ending sort")
  print(nl())
  col(1) "Person"
  if (eq(chc, 5)) { 
    col(30) "Place"
  } else {
    col(30) "Date" 
  }
  col(50) "Spouse"
  col(1)
 	"-----------------------------------------"
 	"-------------------------------------"
  if (eq(chc, 5)) {
    forlist(marriages, fam, n) {
      call display(husband(fam), place(marriage(fam)), wife(fam))
    }
  } elsif (eq(chc, 4)) {
    forlist(marriages, fam, n) {
      call display(husband(fam), date(marriage(fam)), wife(fam))
    }
  } else {
    forindiset(results,husb,val,n) {
      set(first, 1)
      spouses(husb,wife,famv,m) {
        if (first) {
          call display(husb, date(marriage(famv)), wife)
          set(first, 0)
        } else { 
          call display(0, date(marriage(famv)), wife)
        }
      }
      /* display feedback on screen */
      incr(ct)
      incr(ctx)
      if (eq(ctx, rptinterval)) {
        print(d(ct), "I ")
        set(ctx, 0)
      }
    }
  }
  nl()
  print(nl())
}

/*
  Output one result row
*/
proc display(husb, info, wife)
{
/*showframe()*/
  if (husb)
  {
    col(1) fullname(husb, 1,0,29)
  }
  col(30) trim(info, 20)
  if (wife)
  {
    col(50) fullname(wife, 1,0,29)
  }
}

