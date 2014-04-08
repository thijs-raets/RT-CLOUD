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
const int global = 1; 



const int periods[] = {10, 160};
const int periodsSize = 5;


const double approximation = 0.95;
const double maxDeviationPeriods = 0.8;
const int minVMperiod = 1;
const int numberOfCores = 15;
const double maxTaskUtilizationCte = 0.8;


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

class VCPU {
private:
  int _period;
  int _budget;
public:
  VCPU(int period, int budget) :_period(period),_budget(budget) { }
  int getPeriod(){return _period;}
  int getBudget(){return _budget;}
};

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
  std::vector<VCPU*> _VCPUs;
public:
  Domain(string name, double load, int tasksPeriodLBound, int tasksPeriodUBound, string scheduling) : _name(name),_load(load),_tasksPeriodLBound(tasksPeriodLBound),_tasksPeriodUBound(tasksPeriodUBound), _scheduling(scheduling) { }
  ~Domain(void) {
    while(!_tasks.empty()) {
      Task* task = _tasks.back();
      _tasks.pop_back();
      delete task;
    }
  }
  void
  addVCPU(VCPU* vcpu){_VCPUs.push_back(vcpu);}
  
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
  getUtilization(void) { return float(_budget) / float(_period); }

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
  printVCPUs(){
	  std::cout << "#Domain VCPUs: "<< std::endl;
	  for(int i = 0; i < _VCPUs.size(); i++) {
		  int budget = _VCPUs[i]->getBudget();
		  int period = _VCPUs[i]->getPeriod();
		  std::cout << "# p " << period <<", b " << budget << std::endl;
	  }

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
//  string _interfaceXML = "interfaceOut.xml";
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

 


	TiXmlDocument doc;
	doc.LoadFile("interfaceOut.xml");

    TiXmlElement* root = doc.FirstChildElement();
    int i=0;
    for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
    {
	  string elemName = elem->Value();
	  if(elemName.std::string::compare("component")== 0 ){
		  Domain* domain = _domains[i];
		  string domName = elem->Attribute("name");
		  TiXmlNode* processedTasks = elem->LastChild();
		  int j=0;
	          if(global){
			 std::cout << "xl sched-rtglobal -d "<< domName<<" -p 10 -b 20 "<<std::endl;
		   }

		  for(TiXmlElement* task = processedTasks->FirstChildElement(); task != NULL; task = task->NextSiblingElement())
			{
			  std::cout << "xl sched-rtglobal -d "<<domName<<" -v "<<j<<" -p "<<task->Attribute("period") <<" -b "<< task->Attribute("execution_time")<<std::endl;
			  VCPU* vcpu = new VCPU(atoi(task->Attribute("period")),atoi(task->Attribute("execution_time")));
			  domain->addVCPU(vcpu);
			  j++;
			}
		  std::cout << "xl vcpu-set "<< domName<<" "<<j<<std::endl;
		  i++;
	  }
     }

     for(int i = 0; i < _domains.size(); ++i) {
    	//std::cout << "xl sched-rtpartition -d  VM" << (i + 1) << " -b " << _domains[i]->getBudget() << " -p " << _domains[i]->getPeriod() << std::endl;
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


Experiment*
loadTasks(string fileLocation){
	Experiment* experiment = new Experiment(1);

	TiXmlDocument doc;
	doc.LoadFile(fileLocation.c_str());

	  TiXmlElement* root = doc.FirstChildElement();
	  int i=0;
	  for(TiXmlElement* component = root->FirstChildElement(); component != NULL; component = component->NextSiblingElement())
	  {
		  string elemName = component->Value();
		  if(elemName.std::string::compare("component")== 0 ){
			  string domName = component->Attribute("name");
			  string domSched = component->Attribute("scheduler");
			  string domUtilization = component->Attribute("u");
			  string domPeriodMin = component->Attribute("period_min");
			  string domPeriodMax = component->Attribute("period_max");
			  Domain* domain = new Domain(domName,atoi(domUtilization.c_str()), atoi(domPeriodMin.c_str()),atoi(domPeriodMax.c_str()),domSched);

			  int j=0;
			  for(TiXmlElement* task = component->FirstChildElement(); task != NULL; task = task->NextSiblingElement())
				{
				  Task* taskO = new Task(atoi(task->Attribute("e")), atoi(task->Attribute("p")),task->Attribute("name"));
				  domain->addTask(taskO);
				  j++;
				}
			  i++;
			  experiment->addDomain(domain);
		  }
	  }

	  return experiment;
}

int
main(int argc, char** argv)
{
 Experiment* experiment = loadTasks("interfaceIn.xml");
 experiment->dump();

 delete experiment;
 return 0;
}


