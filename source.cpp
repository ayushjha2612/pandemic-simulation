#include<iostream>
#include<vector>
#include<set>
#include<time.h>
#include<queue>

#define Num_Nodes 100                            // NO of nodes

enum coin_t {Heads, Tails};
enum event_type {Infection, Recovery};

struct node_t{
   unsigned int Node_id;                    // 0 to 99 For 100 nodes
   unsigned int TimeStamp;
   event_type Event_type;
   int distance_from_source;
};
typedef struct node_t node;

node population[Num_Nodes];               // Population information

std::vector<int> adj_list[Num_Nodes];         // Graph
std::vector<node> Q;                           // Min heap Q

std::set<int> S;                      // Sets
std::set<int> I;
std::set<int> R;

coin_t toss_coin(){
     int result=rand()%2;
     if(result)   return Tails;
     else         return Heads; 
}

//************    MIN HEAPS IMPLEMENTATION  ************//
int parent(int i) {
        return (i - 1) / 2;
    }
 
 int left(int i) {
        return (2*i + 1);
} 
int right(int i) {
        return (2*i + 2);
}
void swap(node *x, node *y)
{
    node temp = *x;
    *x = *y;
    *y = temp;
}

void Min_Heapify(int i)
{
    int l = left(i);
    int r = right(i);
    int smallest = i;

    if (l < Q.size() && Q[l].TimeStamp < Q[i].TimeStamp)
        smallest = l;
    if (r <Q.size() && Q[r].TimeStamp < Q[smallest].TimeStamp)
        smallest = r;

    if (smallest != i)
    {
        swap(&Q[i], &Q[smallest]);
        Min_Heapify(smallest);
    }
}
  
void InsertQ(node& node){
      
    // First insert the new key at the end
    int i = Q.size() - 1;
    Q.push_back(node);
  
    while (i != 0 && Q[parent(i)].TimeStamp > Q[i].TimeStamp)
    {
       swap(&Q[i], &Q[parent(i)]);
       i = parent(i);
    }
}

node DeletetMinQ()
{
    node root = Q[0];
    if (Q.size() == 1)
    {
        Q.pop_back();
        return root;
    }
  
    // Store the minimum value, and remove it from heap
    Q[0] = Q[Q.size()-1];
    Min_Heapify(0);
    Q.pop_back();

    return root;
}

// *****************  GRAPH FUNCTIONS   *****************//

void create_edge(const unsigned& node_id1 , const unsigned& node_id2 ){

    adj_list[node_id1].push_back(node_id2);
    adj_list[node_id2].push_back(node_id1);
}

void create_graph(){
    coin_t coin;

    for(unsigned i=0 ; i< Num_Nodes; i++){
        for(unsigned j=i ; j < Num_Nodes ; j++){
            coin=toss_coin();
            if(coin==Heads)
                create_edge(i,j);
        }
    }  
}

std::vector<int> get_neighbours(int nodeid){
    return adj_list[nodeid];
}

// ************** BFS OF GRAPH   **************//
void BFS(int source){
     int distance=0;
     std::string space="                           ";
     bool visited[Num_Nodes];
     for(unsigned i=0 ; i< Num_Nodes;i++){
         visited[i]=false;
     }
     std::queue<int> queue;
     visited[source]=true;
     queue.push(source);
     population[source].distance_from_source=distance;
     std::cout << source << space << population[source].TimeStamp  <<space << distance << std::endl;
  

     std::vector<int> neighbours;
     while(!queue.empty()){
        
         source= queue.front();
      
         distance=population[source].distance_from_source;
         queue.pop();
         neighbours=adj_list[source];
         for(unsigned j : neighbours){
             if(!visited[j]){
                 visited[j]=true;
                population[j].distance_from_source=distance+1; 
        
                std::cout << j <<space << population[j].TimeStamp  << space << distance+1 << std::endl;  
             
                queue.push(j);   
             }
         }
     }

}

// *********** FUNCTIONS TO CHECK IF A NODE WAS ALREADY INFECTED OR RECOVERED  ******//

bool already_infected(int nodeid){
    bool result=false;
    for(unsigned i=0; i <Q.size();i++){
        if(Q[i].Node_id==nodeid && Q[i].Event_type==Infection){
             result=true;
             break;
        }

    }
    return result;
}
bool already_recovered(int nodeid){
    bool result=false;
    for(unsigned i=0; i <Q.size();i++){
        if(Q[i].Node_id==nodeid && Q[i].Event_type==Recovery){
             result=true;
             break;
        }
            
    }
    return result;
}
// ********** FUNCTION TO PLOT *********//
void plot(int TimeStamp){  
     std::string space="               ";
     std::cout << TimeStamp <<space << S.size() << space <<I.size() << space << R.size() << std::endl;
}


// ************   MAIN FUNCTION TO START THE PANDEMIC   *********//
int start_pandemic(){

    for(unsigned i=0 ; i< Num_Nodes ;i++){
        S.insert(i);
    }
     int source=rand()%Num_Nodes;           // Source of infection 
     node u;
     u.TimeStamp=0;
     u.Node_id=source;
     u.Event_type=Infection;
     population[source]=u;
     
    InsertQ(u);
    int timestamp=0;

    while(!Q.empty()){
       
        node e=DeletetMinQ();

        if(e.Event_type==Recovery){
         
              
            if(I.find(e.Node_id)!=I.end() && R.find(e.Node_id)==R.end()) {  // If Not present in R and present in I
               I.erase(e.Node_id);
                R.insert(e.Node_id);
            }
        }
         if(e.Event_type==Infection){
                
            if(S.find(e.Node_id)!=S.end()  && I.find(e.Node_id)==I.end()) {  // If Not present in I and present in S
                 I.insert(e.Node_id);
                 S.erase(e.Node_id);
            }  
          //*****    Recovery event for timestamp =0   ******//
            if(e.TimeStamp==0){
               u.Node_id=e.Node_id;
               u.TimeStamp=rand()%6;
               u.Event_type=Recovery;
               InsertQ(u);
               std::cout << "Time Stamp  " << "  Susceptible  " << "  Infected  " << "      Recovered  " << std::endl;
               plot(e.TimeStamp);
              
          }   
            
            std::vector<int> neighbours=get_neighbours(e.Node_id);
            for(int nodeid: neighbours )           // for all susceptible neighbours
            {
                unsigned j;
                bool flag=true;
                if(S.find(nodeid)==S.end())    // If not susceptible
                   continue;
                   
                for( j=1 ; j <=5 ; j++){         // five toss loops
                    coin_t coin =toss_coin();
                    if(coin==Heads){
                          flag=false;
                      
                          break;
                    }
                } 
                if(flag)                      // did not get infected
                    continue;
               
               // ****   Infection event   ********//
                u.Node_id=nodeid;
                u.TimeStamp=    e.TimeStamp+j;
                u.Event_type=Infection;
                population[nodeid]=u;            //   storing the times for infection
                if(!already_infected(nodeid))     // If once infcted dont infect again 
                   InsertQ(u);

                 //******   Recovery event   *******//

                int k= rand()%5 + e.TimeStamp+j+1;
                u.Node_id=nodeid;
                u.TimeStamp= k;
                u.Event_type=Recovery;
                if(!already_recovered(nodeid))      // If once recovered dont recover again 
                   InsertQ(u);
            }
        }
        
         if(timestamp!=e.TimeStamp){           // Plotting according to timestamps 
            plot(e.TimeStamp);
         }
          timestamp=e.TimeStamp;    
    }
    return source;
}


int main(){                                   // int main for calling the functions

    srand(time(0));
    create_graph();

    std::cout << "             *****  STARTING PANDEMIC  *****  \n"<< std::endl;
    std::cout <<"  *** 1.PLOT OF Susceptible, Infected and Recovered against TimeStamp ***\n"<< std::endl; 

    int source=start_pandemic();

    std::cout <<"\n\n  *** 2.COMPARISON OF Infection instant against Shortest Distance from source ***\n"<< std::endl; 
    std::cout << "Node Id  " << "  Time at which node gets affected  " <<"  Shortest distaance from S"<< std::endl;
    
    BFS(source);
    return 0;
}