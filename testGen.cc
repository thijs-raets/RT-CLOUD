#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <time.h>
#include <fstream>
#include <string>
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinyxml.cpp"
#include "tinyxml/tinyxmlerror.cpp"
#include "tinyxml/tinyxmlparser.cpp"
#include "tinyxml/tinystr.cpp"
#include "tinyxml/tinystr.h"


using std::string;
// All units in ms.

const int highestPriority = 99;
const int taskUpperBound = 200000;
const int samples = 100;




const int periods[] = {10, 160};
const int periodsSize = 5;


const double approximation = 0.99;
const double maxDeviationPeriods = 0.8;
const int minVMperiod = 1;
const int numberOfCores = 15;
const double maxTaskUtilizationCte = 0.8;


//const float minimumUtilization = float(1) / float(periods[periodsSize - 1]);

class Task {
private:
  int _executionTime;
  int _period;
  int _priority;
  string _name;
public:
  Task(int executionTime, int period, string name) : _executionTime(executionTime), _period(period), _name(name) { }
  int getExecutionTime(void) { return _executionTime; }
  int getPeriod(void) { return _period; }
  float getUtilization(void) { return float(_executionTime) / float(_period); }
  int getPriority(void) { return _priority; }
  void setPriority(int priority) {
    _priority = priority;
  }

  string getName(){
	  return _name;
  }
  void dump(void) {
    std::cout << "Execution Time " << _executionTime << std::endl;
    std::cout << "Period " << _period << std::endl;
    std::cout << "Priority " << _priority << std::endl;
  }

};

class TaskSorterP {
public:
  bool operator() (Task* x, Task* y) {
    return x->getPeriod() < y->getPeriod();
  }
};

class TaskSorterE {
public:
  bool operator() (Task* x, Task* y) {
    return x->getExecutionTime() > y->getExecutionTime();
  }
};



class Interface {
private:
public:
	Interface(){}
};

class BoundedDelayInterface : public Interface{
private:
	double _utilization;
	int _boundedDelay;
public:
	BoundedDelayInterface(double utilization, double boundedDelay) : _utilization(utilization), _boundedDelay(boundedDelay) { }
	double getUtilization(){return _utilization;}
	int getBoundedDelay(){return _boundedDelay;}
};

class PeriodicInterface : public Interface {
protected:
	int _period;
	int _executionTime;
public:
	PeriodicInterface(int period, int executionTime) : _period(period),_executionTime(executionTime) { }
	int getExecutionTime(void) { return _executionTime; }
	 int getPeriod(void) { return _period; }
};

class MPR : public PeriodicInterface {
private:
  int _m;
public:
 // MPR(int period, int executionTime, int m) :  _period(period), _executionTime(executionTime), _m(m) { }
  MPR(int period, int executionTime, int m) :
	  PeriodicInterface(period, executionTime ) {_m = m; }
  int getConcurrencyBound(){return _m;}
};


/*class Cluster {
private:
  int _concurrencyBound;
  std::vector<Task*> _tasks;
public:
  Cluster(int concurrencyBound, std::vector<Task*> tasks) : _concurrencyBound(concurrencyBound), _tasks(tasks) { }
  int getConcurrencyBound(void) { return _concurrencyBound; }
};*/

class Domain {
private:
  int _budget;
  int _period;
  double _load;
  int _tasksPeriodLBound;
  int _tasksPeriodUBound;
  int _concurrencyBound;
  string _scheduling;
  string _name;
  std::vector<Task*> _tasks;

  // Task(int executionTime, int period) : _executionTime(executionTime), _period(period) { }
public:
  Domain(string name, double load, int tasksPeriodLBound, int tasksPeriodUBound,int concurrencyBound, string scheduling) : _name(name),_load(load),_tasksPeriodLBound(tasksPeriodLBound),_tasksPeriodUBound(tasksPeriodUBound), _concurrencyBound(concurrencyBound), _scheduling(scheduling) { }
  ~Domain(void) {
    while(!_tasks.empty()) {
      Task* task = _tasks.back();
      _tasks.pop_back();
      delete task;
    }
  }
  
  string
  getName(){return _name;}

  string
  getScheduling(){return _scheduling;}

  int
  getBudget(void) { return _budget; }

  int
  getPeriod(void) { return _period; }

  double
  getLoad(void){return _load;}

  int
  getTasksPeriodLBound(void) { return _tasksPeriodLBound; }

  int
  getTasksPeriodUBound(void) { return _tasksPeriodUBound; }


  void
  setPeriod(int period){_period = period;}

  void
  setBudget(int period){_budget = period;}

  float
  getUtilization(void) { return _load; }

  void
  addTask(Task* task) { _tasks.push_back(task); }
  
  void
  printTasks(){
	  std::cout << "#Domain with load:  " << _load << ", tasks:"<< std::endl;
	  double workload;
	  for(int i = 0; i < _tasks.size(); i++) {
		  int executionTime = _tasks[i]->getExecutionTime();
		  int period = _tasks[i]->getPeriod();
		  std::cout << "# e " << executionTime <<", p " << period<< std::endl;
		  workload += (double)(executionTime)/period;
	  }
	  std::cout << "# workload:  " << workload << std::endl;
  }


  void
  assignPriorities(void) {
     std::sort(_tasks.begin(), _tasks.end(), TaskSorterP());
     int priority = highestPriority;
     _tasks[0]->setPriority(priority);

     for(int i = 1; i < _tasks.size(); ++i) {
       if(_tasks[i - 1]->getPeriod() != _tasks[i]->getPeriod()) {
 	--priority;
       }
       _tasks[i]->setPriority(priority);
     }
   }

  std::vector<Task*>
  getTasks(){
	  return _tasks;
  }

  bool empty(void) { return _tasks.empty(); }

  void dump(int experimentNumber, int id, int duration) {
    for(int i = 0; i < _tasks.size(); ++i) {
      std::cout << "echo './period " << _tasks[i]->getExecutionTime() << " " << _tasks[i]->getPeriod() << " " << 1 << " " << "120000" << " " << _tasks[i]->getPriority() << " > " << experimentNumber << "/" << id << "_" << i << ".trace &' >> " << experimentNumber << "/" << id << ".sh" << std::endl;
      std::cout << "echo '" << experimentNumber << " " << id << " " << i << " " << _tasks[i]->getExecutionTime() << " " << _tasks[i]->getPeriod() << " " << _tasks[i]->getUtilization() << "' >> " << experimentNumber << "/tasksummary" << std::endl;
    }

    std::cout << "echo 'wait' >> " << experimentNumber << "/" << id << ".sh" << std::endl;
    std::cout << "echo 'rm todo/" << id << "' >> " << experimentNumber << "/" << id << ".sh" << std::endl;
    std::cout << "echo 'touch done/" << id << "' >> " <<  experimentNumber << "/" << id << ".sh" << std::endl;
  }

  void dump1(int experimentNumber, int id, int duration) {
    std::cout << "echo '1 " << id << " " << getBudget() << " " << getPeriod() << "' >> " << experimentNumber << "/totalsummary" << std::endl;
    for(int i = 0; i < _tasks.size(); ++i) {
      std::cout << "echo '2 " << id << " " << _tasks[i]->getExecutionTime() << " " << _tasks[i]->getPeriod() << " " << _tasks[i]->getUtilization() << "' >> " << experimentNumber << "/totalsummary" << std::endl;
    }
  }
   
};

class Experiment {
private:
  int _experimentNumber;
  std::vector<Domain*> _domains;
public:
  Experiment(int experimentNumber) : _experimentNumber(experimentNumber) { }
  ~Experiment(void) {
    while(!_domains.empty()) {
      Domain* domain = _domains.back();
      _domains.pop_back();
      delete domain;
    }
  }
  
  void addDomain(Domain* domain) {
    _domains.push_back(domain);
  }

  std::vector<Domain*> getDomains(){
	  return _domains;
  }
  float getTotalUtilization(void) {
    float total = 0.0;
    for(int i = 0; i < _domains.size(); ++i) {
      total += _domains[i]->getUtilization();
    }
    return total;
  }

  int getDuration(void) {
    int retval = 0;
    for(int i = 0; i < _domains.size(); ++i) {
     // int t = _domains[i]->longestPeriod();
     // retval = (retval > t) ? retval : t;
    }
    return retval * samples;
  }

  void dump(void) {

    std::cout << "mkdir -p " << _experimentNumber << std::endl;

    int d = getDuration();

    for(int i = 0; i < _domains.size(); ++i) {
      std::cout << "echo '" << (i + 1) << " " << _domains[i]->getBudget() << " " << _domains[i]->getPeriod() << " " << _domains[i]->getUtilization() << "' >> " << _experimentNumber << "/domainsummary" << std::endl;
    }

    for(int i = 0; i < _domains.size(); ++i) {
      _domains[i]->dump(_experimentNumber, i + 1, d);
    }

    for(int i = 0; i < _domains.size(); ++i) {
      _domains[i]->dump1(_experimentNumber, i + 1, d);
    }

 

    for(int i = 0; i < _domains.size(); ++i) {
    	std::cout << "xl sched-rtpartition -d  VM" << (i + 1) << " -b " << _domains[i]->getBudget() << " -p " << _domains[i]->getPeriod() << std::endl;
    	//std::cout << "sudo xm sched-rt -d vm" << (i + 1) << " -b " << _domains[i]->budget() << " -p " << _domains[i]->period() << std::endl;
    }

    for(int i = 0; i < _domains.size(); ++i) {
      std::cout << "echo '" << _experimentNumber << "' > todo/" << (i + 1) << std::endl;
    }

    for(int i = 0; i < _domains.size(); ++i) {
      std::cout << "while [ ! -e done/" << (i + 1) << " ]; do sleep 1; done" << std::endl;
    }

    for(int i = 0; i < _domains.size(); ++i) {
      std::cout << "rm done/" << (i + 1) << std::endl;
    }

  }
};


bool
checkPeriod(int periodNew, int period){
	bool ok = 1;
	//for(int i = 0; i < tasks.size(); i++){
		//int currentPeriod = tasks[i]->getPeriod();
		double upperLimit = (1 + maxDeviationPeriods)*period;

		double downLimit = (1-maxDeviationPeriods)*period;
		if(periodNew > upperLimit || periodNew < downLimit){
			ok = 0;
		}
	//}
	return ok;
}

void
createXMLinput(string file, Experiment* experiment){
	TiXmlDocument doc;
	 TiXmlElement* root = new TiXmlElement("system");
	 doc.LinkEndChild(root);
	 root->SetAttribute("os_scheduler","gEDF");
	 root->SetAttribute("period","16.0");

	 std::vector<Domain*> domains = experiment->getDomains();
	 for(int i = 0; i <domains.size(); i++) {
		 Domain* domain = domains[i];
		 TiXmlElement* componentEl = new TiXmlElement("component");
		  root->LinkEndChild(componentEl);
		  componentEl->SetAttribute("scheduler",domain->getScheduling().c_str());
		  componentEl->SetAttribute("name",domain->getName().c_str());
		  componentEl->SetAttribute("period","64");
		  
		  char numstr[21];
		  sprintf(numstr, "%f", domain->getUtilization());
		  componentEl->SetAttribute("u",numstr);

		  sprintf(numstr, "%d", domain->getTasksPeriodLBound());
		  componentEl->SetAttribute("period_min",numstr);

		  sprintf(numstr, "%d", domain->getTasksPeriodUBound());
		  componentEl->SetAttribute("period_max",numstr);

		  std::vector<Task*> tasks = domain->getTasks();
		  for(int i = 0; i <tasks.size(); i++) {
			  Task* task = tasks[i];
			  TiXmlElement* taskEl = new TiXmlElement("task");
			  componentEl->LinkEndChild(taskEl);


			  int period = task->getPeriod();
			  int executionTime = task->getExecutionTime();
			  char numstr[21];
			  sprintf(numstr, "%d", period);

		      char numstr2[21];
		      sprintf(numstr2, "%d", executionTime);


		      taskEl->SetAttribute("name",task->getName().c_str());
			  taskEl->SetAttribute("p",numstr);
			  taskEl->SetAttribute("d",numstr);
			  taskEl->SetAttribute("e",numstr2);
			  taskEl->SetAttribute("delta_rel","0");
			  taskEl->SetAttribute("delta_sch","0");
			  taskEl->SetAttribute("delta_cxs","0");
			  taskEl->SetAttribute("delta_crpmd","0.9");


		  }

	 }
	 doc.SaveFile(file.c_str());
}

int
main(int argc, char** argv)
{

  int experimentNumber;// = atoi(argv[1]);
  int seed;// = atoi(argv[2]);
  string resourceInterfaceAbstraction = "MPR";// = argv[3];
  string schedulingAlgorithm = "RM"; // = argv[4];

  experimentNumber = 1;
  seed = 33333;
  Experiment* experiment = new Experiment(experimentNumber);
  srand48(seed);

  //srand(seed);

  //when using MPR we hold: domain = cluster
  std::vector<Domain*> domains;
  Domain* domain1 = new Domain("VM3",3.5, 700,800,numberOfCores-2,"gEDF");
  Domain* domain2 = new Domain("VM6",2.6, 600,700,numberOfCores-3,"gEDF");
  domains.push_back(domain1);
  domains.push_back(domain2);
  int counter = 0;

  for(int i = 0; i <domains.size(); i++) {
	 double currentDomainLoad = 0;
	 Domain* domain = domains[i];
	 std::vector<Task*> tasks;
	 double maxLoad = domain->getLoad();
	 int minTaskPeriod = domain->getTasksPeriodLBound();
	 int maxTaskPeriod = domain->getTasksPeriodUBound();
	 int firstPeriod;
	 double maxTaskUtilization;
	 int counter2 = 0;
	 if(maxLoad <= 0.9){
		 maxTaskUtilization = maxLoad;
	 }else{
		 maxTaskUtilization = maxTaskUtilizationCte;
	 }


	 do{
		 //int period =  (int) (minTaskPeriod + (rand()/(double)((RAND_MAX + 1))*(maxTaskPeriod - minTaskPeriod)));
		 int period =  (int) (minTaskPeriod + drand48()*(maxTaskPeriod-minTaskPeriod));
		 if (counter2 == 0){
			 firstPeriod = period;
		 }

		 //we want our random generated period to be restricted to a certain range with respect to the first period generated
		 while(!checkPeriod(period, firstPeriod)){
			 //period = (int) (minTaskPeriod + (rand()/(double)((RAND_MAX + 1))*(maxTaskPeriod - minTaskPeriod)));
			  period =  (int) (minTaskPeriod + drand48()*(maxTaskPeriod-minTaskPeriod));
		 }



		// int executionTime = (int) (1 + maxTaskUtilization*period*(1 - (rand()/(double)(RAND_MAX + 1))));
		 int executionTime = (int) (1 + maxTaskUtilization*period*(1-  drand48()));
		 string t = "T";
		 char numstr[21];

		 sprintf(numstr, "%d", counter);
		 t+=numstr;
		 Task* task = new Task(executionTime, period,t);
		 double taskUtilization = ((double)executionTime)/period;
		 currentDomainLoad += taskUtilization;
		 if(currentDomainLoad < maxLoad){
			 counter++;
			 counter2++;
			 tasks.push_back(task);
			 domain->addTask(task);
		 }else{
			 currentDomainLoad -= taskUtilization;
		 }
	 }while((currentDomainLoad < maxLoad*approximation) );

	 domain->assignPriorities();
	 domain->printTasks();

	 experiment->addDomain(domain);

  }

  createXMLinput("interfaceIn.xml", experiment);

  delete experiment;

  return 0;
}


