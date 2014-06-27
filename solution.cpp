#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;
#endif /* __PROGTEST__ */

struct item{
   string name;
   string area;
   bool added;
   item* next;
   item(){next = NULL;added = false;}
   item(const string& n, const string& a){name=n;area=a;next=NULL;added=true;}
   void add_item(const string& n, const string& a){
      name = n;
      added = true;
      area = a;
   }
};
struct Unknown{
   string name;
   Unknown* next;
   Unknown(const string name):name(name),next(NULL){}
};
struct Data{
   string m_area;
   string m_name;
   unsigned int m_amount;
   Unknown* next;
   Data(const string& a, const string& n){m_area=a;m_amount=1;m_name=n;next=NULL;}
   ~Data(){
      Unknown* to_del,*tmp;
      tmp = next;
      while(tmp){
         to_del = tmp;
         tmp = tmp->next;
         delete to_del;
      }
   }
   void  add(const string& name){
      Unknown* tmp;
      //if(name == m_area)
      //   return;
      //if(next)
      //   if(name == next->name)
      //      return;
      //tmp = next;
      //while(tmp){
      //   if(tmp->name == name)
      //      return;
      //   tmp = tmp->next;
      //}
      tmp = new Unknown(name);//Když již štěnice je v unknown a přijde další požadavek na její přidání, tak se nic neprovede.
                              //Tj. v unknown zůstává a nemění se žádné počty.
      tmp-> next = next;
      next = tmp;
      ++m_amount;
   }
   bool  delete_node(const string& name){
      Unknown* tmp,*prev;
      if(name == m_name){
         m_name = next->name;
         tmp = next->next;
         next = tmp;
         --m_amount;
         return true;
      }
      prev = tmp = next;
      if(tmp->name == name){
         tmp = tmp->next;
         delete next;
         next = tmp;
         --m_amount;
         return true;
      }
      while(tmp){
         if(tmp->name == name){
            prev->next = tmp->next;
            delete tmp;
            --m_amount;
            return true;
         }
         prev = tmp;
         tmp = tmp->next;
      }
      return false;
   }
};
class CDatabase{
   unsigned int table_size;
   int unknown;
   item* hash_table;
   Data** AR;
   bool flag;
   unsigned int MAX;
   unsigned int am_AR;
  public:
          CDatabase    ( );
         ~CDatabase    ( );
   void   AddBug       ( const string    & name, const string    & area );
   void   AddArea      ( const string    & name, const string    & area );
   bool   DelArea      ( const string    & name, const string    & area );
   void   DelBug       ( const string    & name );
   void   Print        ( ostream         & os ) const;
   unsigned int Hash   ( const string    & name);
};


CDatabase::~CDatabase(){
      for(unsigned int i = 0;i < am_AR; ++i)
         delete AR[i];
      delete []AR;
      for(unsigned int i = 0;i < table_size; ++i){  
         if(hash_table[i].next){
            item* to_del;
            item* tmp;
            to_del = tmp = hash_table[i].next;
            while(tmp){
               to_del = tmp;
               tmp = tmp->next;
               delete to_del;
               to_del = NULL;
            }
         }
      }
      delete []hash_table;
}
CDatabase::CDatabase (){
   table_size = 131071;//17659;//43261;//96557;
   am_AR = unknown = 0;
   flag = false;
   MAX = 100000;
   AR = NULL;
   hash_table = NULL;
}
unsigned int CDatabase::Hash(const string& name){
   unsigned int b    = 378551;
   unsigned int a    = 63689;
   unsigned int hash = 0;
   for(unsigned int i = 0; i < name.length(); i++){
      hash = hash * a + name[i];
      a    = a * b;
   }
   return hash % table_size;
}
void   CDatabase::Print(ostream& os ) const{
   for(unsigned int i = 0; i < am_AR; ++i)
      os << AR[i]-> m_area<<' '<< AR[i]-> m_amount<<endl;
   if(unknown)
      os<<"unknown "<<unknown<<endl;
}
bool CDatabase::DelArea(const string& name, const string& area){
   if(!am_AR)
      return false;
   int low=0;
   int high = am_AR-1;
   int mid; 
   while(low <= high){
      mid = (low+high)/2;
      if(area == AR[mid]-> m_area){
         if(AR[mid]-> m_amount>1){
            if(!AR[mid]->delete_node(name))
               return false;
            return true;
         }
         if(name != AR[mid]->m_name)
            return false;
         delete AR[mid];
         for(unsigned int i = mid; i < am_AR-1; i++)
            AR[i] = AR[i+1];
         AR[am_AR-1] = NULL;
         --am_AR;
         return true;
      }
      else if(area > AR[mid]-> m_area)
         low=mid+1;
      else
         high=mid-1;
   }
   return false;
}
void CDatabase::AddArea(const string& name, const string& area){
   if(!am_AR){
      AR[0] = new Data(area, name);
      ++am_AR;
      return;
   }
   int low=0;
   int high=am_AR-1;
   int mid;
   while(low<=high){
      mid = (low+high)/2;
      if(area == AR[mid]->m_area){
         AR[mid]->add(name);
         return;
      }
      else if(area>AR[mid]-> m_area)
         low=mid+1;
      else
         high=mid-1;
   }
   for(int i=am_AR;i>low;i--)
      AR[i]=AR[i-1];
   AR[low]=new Data(area, name);
   ++am_AR;
}

void CDatabase::AddBug(const string& name, const string& area){
   if(!flag){
      AR = new Data* [MAX];
      hash_table = new item [table_size];
      flag  = true;
   }
   if(am_AR == MAX-1){
         Data** t;
         t = new Data*[MAX];
         for(unsigned int i = 0; i < am_AR; i++)
            t[i] = AR[i];
         delete [] AR;
         MAX*=10;
         AR = new Data* [MAX];
         for(unsigned int i = 0; i < am_AR; i++)
            AR[i] = t[i];
         delete [] t; 
   }
   unsigned int index=Hash(name);
   item* tmp;
   if(!hash_table[index].added){
       hash_table[index].add_item(name,area);
       AddArea(name, area);
       return;
   }
   if(name == hash_table[index].name && area == hash_table[index].area)
      return;
   if(name == hash_table[index].name){
      if(DelArea(name, hash_table[index].area))
         ++unknown;
      return;
   }
   if(hash_table[index].next){
      tmp = hash_table[index].next;
      if(tmp->name == name){
         if(tmp->area == area)
            return;
         if(DelArea(name, tmp->area))
            ++unknown;
         return;
      }
      while(tmp != NULL){
         if(tmp-> name == name){
            if(tmp->area == area)
               return;
            if(DelArea(name, tmp->area))
               ++unknown;
            return;
         }
         tmp = tmp-> next;
      }
   }
   tmp = new item(name,area);
   tmp->next = hash_table[index].next;
   hash_table[index].next = tmp;
   AddArea(name, area);
}

void CDatabase::DelBug(const string& name){
   unsigned int index = Hash(name);
   item* tmp;
   item* prev;
   if(!hash_table[index].added)
      return;
   if(name == hash_table[index].name){
      if(!DelArea(name, hash_table[index].area))
         --unknown;
      hash_table[index].name = "";
      hash_table[index].added= false;
      hash_table[index].area = "";
      if(hash_table[index].next){
         prev = tmp = hash_table[index].next;
         hash_table[index].add_item(tmp->name, tmp->area);
         tmp = tmp->next;
         delete prev;
         hash_table[index].next = tmp;
      }
      return;
   }
   prev = tmp = hash_table[index].next;
   if(hash_table[index].next){
      if(tmp->name == name){
         if(!DelArea(name, tmp->area))
            --unknown;
         else{
            tmp->name = "";
            tmp->added= false;
            tmp->area = "";
         }
         tmp = tmp->next;
         delete prev;
         hash_table[index].next = tmp;
         return;
      }
   }
   while(tmp != NULL){
      if(tmp-> name == name){
         if(!DelArea(name, tmp-> area))
            --unknown;
         else{
            tmp->name = "";
            tmp->added= false;
            tmp->area = "";
         }
         prev->next = tmp->next;
         delete tmp;
         return;
      }
      prev = tmp;
      tmp = tmp-> next;
   }
}





#ifndef __PROGTEST__
int main ( void )
 {
   CDatabase db;
   char op;
   string name, area;
   
   while ( cin >> op >> name )
    {
      if ( op == '+' )  
       {
         cin >> area;
         db . AddBug ( name, area );
       }
      if ( op == '-' )   
       db . DelBug ( name );
    }
   db . Print ( cout );
   
   return 0; 
 }
#endif /* __PROGTEST__ */
