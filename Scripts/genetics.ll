global(twin_table)

proc main() {

table(twin_table)

/* Get the first person and ancestors and multiplicities */
    set(p1, indi("i1"))
    table(anc1_table)
    set(kp1,save(key(p1)))
	showframe()
"calling recur_anc and anc1_table is: " valueof(anc1_table) "\n"
    call recur_anc(kp1, 0, anc1_table, 0)
	return() /* GET OUT QUICK FOR TESTING */

/* Get the second person and ancestors and relatedness to common ancestors. */
    set(p2, indi("i100"))
    list(lca_list)
    table(lca_table)
    set(kp2,save(key(p2)))
    call recur_anc(kp2,lca_list,lca_table,anc1_table)

/* Now calculate relations */
    if (length(lca_list)) {
        print(kp1," ",name(indi(kp1))," is\n",kp2," ",name(indi(kp2)),"'s\n")
        list(gsums)
        set(gmax,0)
        forlist(lca_list,lca,ilca) {
            set(ll,lookup(anc1_table,lca))
            /*set(gl1,getel(ll,1))*/ set(gl1,getel(ll,0))
            /*set(kl1,getel(ll,2))*/ set(kl1,getel(ll,1))
            set(ll,lookup(lca_table,lca))
            /*set(gl2,getel(ll,1))*/ set(gl2,getel(ll,0))
            /*set(kl2,getel(ll,2))*/ set(kl2,getel(ll,1))
            forlist(gl1,g1,il1) {
                set(k1,getel(kl1,il1))
                forlist(gl2,g2,il2) {
                    set(k2,getel(kl2,il2))
                    call print_rel(kp1,k1,k2,g1,g2)
                    set(gsum,add(g1,g2))
                    enqueue(gsums,gsum)
                    if (gt(gsum,gmax)) { set(gmax,gsum) }
                }
            }
        }
    }
    else {
        print(kp1," ",name(indi(kp1))," and ",kp2," ",name(indi(kp2)),
                " are not related by blood.\n")
        return()
    }

/* Add up path weights */
    set(gsum,0)
    forlist(gsums,g,gnum) {
        set(gpow,1)
        while(lt(g,gmax)) {
            set(gpow,add(gpow,gpow))
            incr(g)
        }
        set(gsum,add(gsum,gpow))
    }
/* Cancel common factors of 2 */
    if (gsum) {
        while (not(mod(gsum,2))) {
            set(gsum,div(gsum,2))
            decr(gmax)
        }
    }
/* Figure common denominator */
    set(gpow,1)
    while(gmax) {
        set(gpow,add(gpow,gpow))
        decr(gmax)
    }
/* Print out final answer */
    print("Expected degree of genetic overlap: ",d(gsum),"/",d(gpow),"\n")
}

/* This is the magic routine that does the real work.
   If there is no input stop_table, calculate all the
   ancestors along all paths of the input person, and return
   the ancestors and their multiplicities.
   If there is an input stop_table, calculate the ancestors
   up to the ones contained in the stop table, and return
   only the ones in the stop table and their multiplicities.
   Notes: If there were a fortable() iterator, then the anc_list
   would be unnecessary.  The fake keys are used to simulate
   ancestors who aren't explicitly in the database.
   The table entries are lists of two elements.  The first element
   is a list of generation counts for a path to that ancestor or
   his or her twin, the second element is a list of actual keys of the
   ancestor.  These actual keys differ only if the ancestor is a twin.
   If the ancestor is a twin, the key to the table entry is the key of
   the "oldest" twin.
*/
proc recur_anc(kp, anc_list, anc_table, stop_table) {
"recur_anc just called and anc_table is : " valueof(anc_table) "\n"
    list(keys)
    list(gens)         /*DEBUG*/"1.  recur_anc called: stack is: " showframe() "\n"
    enqueue(keys,kp)
    enqueue(gens,0)   /*DEBUG*/ "2.  keys: " valueof(keys) "   gens: " valueof(gens) "\n" set(debug_count, 0)
                      
    while (ka,dequeue(keys)) {    /*DEBUG*/ "2.5.  Dequeue number " set(debug_count, add(1, debug_count)) d(debug_count) ".\n"
        set(g,dequeue(gens))
        set(k,first_twin(ka))     /*DEBUG*/ "3.  g: "valueof(g) "  k: " valueof(k) "  stop_table: "valueof(stop_table) "\n"
        if (stop_table) {
          set(stop,lookup(stop_table,k))
        }
        if (or(not(stop_table),stop)) {    /*DEBUG*/ "4. anc_table: " set(xyz, lookup(anc_table, k)) "contents are " valueof(xyz) nl()
            if (ll,lookup(anc_table,k)) {  /*DEBUG*/ "5. We are in the then branch.?\n"
                /*set(l,getel(ll,1))*/ set(l,getel(ll,0))
                /*set(kl,getel(ll,2))*/ set(kl,getel(ll,1))
            }
            else {  /*DEBUG*/ "6. In the else branch.\n" "ll = " valueof(ll) "  l = " valueof(l) "  kl = " valueof(kl) "\n"
                list(ll)                      /*DEBUG*/ "7.  list(ll)\n"
                list(l)                       /*DEBUG*/ "8.  list(l); does kl exist before declartion: " valueof(kl) "\n"
                list(kl)                      /*DEBUG*/ "9.  list(kl)\n"
                enqueue(ll,l)                 /*DEBUG*/ "10. enqueue(ll, l)\n"
                enqueue(ll,kl)                /*DEBUG*/ "11. enqueue(ll, kl)\n"
                insert(anc_table, k, ll)      /*DEBUG*/ "12. insert(anc_table, k, ll)\n"
/*DEBUG*/  "anc_table now holds " d(length(anc_table)) " lists\n"
                if (anc_list) { enqueue(anc_list,k) }
            }
            enqueue(l,g)
            enqueue(kl,ka)
        }
        if (not(stop)) {
            if (a,indi(k)) {
                incr(g)
                if (par,parents(a)) {
                    if (aa,father(a)) {
                        enqueue(keys,save(key(aa)))
                    }
                    else {
                        enqueue(keys,save(concat("H0",key(par)))) /* fake */
                    }
                    if (aa,mother(a)) {
                        enqueue(keys,save(key(aa)))
                    }
                    else {
                        enqueue(keys,save(concat("W0",key(par)))) /* fake */
                    }
                    enqueue(gens,g)
                    enqueue(gens,g)
                }
            }
        }
    }
}

proc print_rel(kp1,k1,k2,g1,g2) {
    set(p1,indi(kp1))
    if (lt(g1,g2)) { set(deg,g1) set(rem,sub(g2,g1)) }
    else           { set(deg,g2) set(rem,sub(g1,g2)) }
    if (strcmp(k1,k2)) {
        incr(deg)  /* twin ancestors */
        set(halftwin,"twin-")
    } else { set(halftwin,"half-") }
    if (eq(deg,0)) {
        if (eq(rem,0)) { print("self") }
        else {
            while (gt(rem,2)) { print("g") decr(rem) }
            if (gt(rem,1)) { print("grand") }
            if (gt(g1,g2)) {
/*              print("half-") */
                if (male(p1)) { print("son") }
                elsif (female(p1)) { print("daughter") }
                else { print("child") }
            }
            else {
                if (male(p1)) { print("father") }
                elsif (female(p1)) { print("mother") }
                else { print("parent") }
            }
        }
    }
    elsif (eq(deg,1)) {
        if (eq(rem,0)) {
            print(halftwin)
            if (male(p1)) { print("brother") }
            elsif (female(p1)) { print("sister") }
            else { print("sibling") }
        }
        else {
            while (gt(rem,2)) { print("g") decr(rem) }
            if (gt(rem,1)) { print("grand") }
            if (gt(g1,g2)) {
                print(halftwin)
                if (male(p1)) { print("nephew") }
                elsif (female(p1)) { print("niece") }
                else { print("niece/nephew") }
            }
            else {
                if (male(p1)) { print("uncle") }
                elsif (female(p1)) { print("aunt") }
                else { print("aunt/uncle") }
            }
        }
    }
    else {
        print(ord(sub(deg,1))," ",halftwin,"cousin")
        if (eq(rem,1)) { print(" once") }
        elsif (eq(rem,2)) { print(" twice") }
        elsif (eq(rem,3)) { print(" thrice") }
        elsif (gt(rem,3)) { print(" ",card(rem)," times") }
        if (rem) { print(" removed") }
    }
    print("\n  via their ancestor ",k1," ")
    if (p1,indi(k1)) { print(name(p1)) }
    else {
        print("Unknown ")
        if (strcmp(substring(k1,1,1),"H")) { print("wife") }
        else { print("husband") }
        print(" in family ",substring(k1,3,strlen(k1)))
    }
    if (strcmp(k1,k2)) {
        print("\n  and           twin ",k2," ",name(indi(k2)))
    }
    print("\n")
}

func first_twin(pkey) {
    if (tkey,lookup(twin_table,pkey)) { return(tkey) }
    set(ft,0)
    if (p,indi(pkey)) {
        if (parents(p)) {
            if (b,birbapyear(p)) {
                set(loop,1)
                while(loop) {
                    set(loop,0)
                    if (q,prevsib(p)) {
                        if (not(strcmp(sex(p),sex(q)))) {
                            if (eq(b,birbapyear(q))) {
                                print(key(p)," ",name(p),
                                        birbapdate(p),"    and\n")
                                print(key(q)," ",name(q),
                                        birbapdate(q))
                                /*getint(rt,
                   "Are these individuals identical twins? (0=no, 1=yes)")
                                if (rt) {
                                    set(p,q)
                                    set(loop,1)
                                    set(ft,p)
                                    print("    are twins\n\n")
                                }
                                else { print("    are not twins\n\n") } */
                            }
                        }
                    }
                }
            }
        }
    }
    if (ft) { set(tkey,save(key(ft))) } else { set(tkey,pkey) }
    insert(twin_table,pkey,tkey)
    return(tkey)
}

func birbapyear(person) {
    if (b,birth(person)) {
        if (byear,atoi(year(b))) { return(byear) }
    }
    if (b,baptism(person)) {
        if (byear,atoi(year(b))) { return(byear) }
    }
    return(0)
}

func birbapdate(person) {
    if (b,birth(person)) {
        if (byear,atoi(year(b))) { return(concat(" born ",date(b))) }
    }
    if (b,baptism(person)) {
        if (byear,atoi(year(b))) { return(concat(" bapt ",date(b))) }
    }
    return("")
}
