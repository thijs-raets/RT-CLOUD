package edu.penn.rtg.experiment.tasksetgenerator;

public class VCPU {
	private double period;
	private double exe;
	private double deadline;
	private int domAffinity; //which dom this vcpu belongs to
	private int cpu; //only used for partitioned algorithm. not consider cluster yet 
	
	public VCPU(){
		this.period = -1;
		this.exe = -1;
		this.deadline = -1;
		this.domAffinity = -1; 
		this.cpu = -1;
	}
	
	public VCPU(String period, String exe, String deadline, int domAffinity){
		this.period = Double.parseDouble(period) ;
		this.exe = Double.parseDouble(exe);
		this.deadline = Double.parseDouble(deadline);
		this.domAffinity = domAffinity;
		this.cpu = -1;
	}
	
	
	public double getPeriod() {
		return period;
	}
	public void setPeriod(double period) {
		this.period = period;
	}
	public double getExe() {
		return exe;
	}
	public void setExe(double exe) {
		this.exe = exe;
	}
	public double getDeadline() {
		return deadline;
	}
	public void setDeadline(double deadline) {
		this.deadline = deadline;
	}
	public int getCpu() {
		return cpu;
	}
	public void setCpu(int cpu) {
		this.cpu = cpu;
	}

	public int getDomAffinity() {
		return domAffinity;
	}

	public void setDomAffinity(int domAffinity) {
		this.domAffinity = domAffinity;
	}
	
	
	
}
