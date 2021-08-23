#include "main.h"
#include "cache.h"
Cache::Cache(SearchEngine* s,ReplacementPolicy* r):rp(r),s_engine(s) {}
Cache::~Cache(){
	delete rp;
	delete s_engine;
}



Data* Cache::read(int addr) {

        int te2 = s_engine->search(addr);
        if (te2 == -1)
            return NULL;
        else
        {
            rp->access(te2);
            s_engine->update_index(rp);

            te2 = s_engine->search(addr);

            return rp->getValue(te2)->data;
        }


}
Elem* Cache::put(int addr, Data* cont) {
    int Count = rp->getCount();
    if (Count < MAXSIZE)
    {
        s_engine->insert(addr, rp->getCount());
        rp->insert(new Elem(addr,cont,true), rp->getCount());
        s_engine->update_index(rp);

    }
    else if(rp->getCount() ==MAXSIZE)
    {
        Elem* temp = rp->dequeuevalue();

        s_engine->deleteNode(temp->addr);
        rp->remove();

        s_engine->update_index(rp);
        s_engine->insert(addr, rp->getCount());     
        rp->insert(new Elem(addr,cont,true), rp->getCount());
        s_engine->update_index(rp);

        return temp;
    }
    return NULL;
}
Elem* Cache::write(int addr, Data* cont) {

    int te2 = s_engine->search(addr);
    if (te2 != -1)// NOT found
    {
        rp->replace(te2,new Elem(addr,cont,false));
        s_engine->update_index(rp);
        return NULL;
    }
    else
    {
        
        if (rp->getCount() < MAXSIZE)
        {            
            s_engine->insert(addr, rp->getCount());
            rp->insert(new Elem(addr, cont, false), rp->getCount());
            s_engine->update_index(rp);

            return NULL;
        }
        else if (rp->getCount() == MAXSIZE)
        {
            Elem* temp = rp->dequeuevalue();

            s_engine->deleteNode(temp->addr);
            rp->remove();
            s_engine->update_index(rp);


            s_engine->insert(addr, rp->getCount());
            rp->insert(new Elem(addr, cont, false),rp->getCount() );
            s_engine->update_index(rp);

            return temp;

        }
    }

    return NULL;
}
void Cache::printRP() {
    rp->print();
}
void Cache::printSE() {
    s_engine->print(rp);
}
