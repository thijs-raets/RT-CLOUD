package edu.penn.rtg.experiment.tasksetgenerator;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.text.DecimalFormat;
import java.util.HashMap;
import java.util.Vector;


import edu.penn.rtg.common.Tool;

/**
 * Class SchedulableTasksetCounter
 * This class is to count the interface's paramter of the calculated schedulable system. 
 * It can parse all xml file to get the top component's interface and uses this to get the average bandwdith v.s. taskset utilization, 
 * each taskset's utilization v.s. taskset index, e.g., 0.10-0
 * 
 * @version 1.0
 * @author Meng Xu
 * @since 4/19/2013
 *
 */
public class SchedulableTasksetCounter {
	public static final int[] whichApproaches = {GlobalVariable.MPR2, GlobalVariable.MPR2hEDF, GlobalVariable.CAMPR2hEDF_TASK_CENTRIC, GlobalVariable.CAMPR2hEDF_MODEL_CENTRIC, GlobalVariable.CAMPR2hEDF_COMBINED};

//	private double tasksetUtil_min;
//	private double tasksetUtil_step;
//	private double tasksetUtil_max; //include
	Vector<Vector<MPRInterface>> mPRInterfaces;
	int whichApproach;

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		if(args.length < 2){
			SchedulableTasksetCounter.printUsage();
			System.exit(0);
		}
		
		
		double tasksetUtil_min = Double.parseDouble(args[0]);
		double tasksetUtil_step = Double.parseDouble(args[1]);
		double tasksetUtil_max = Double.parseDouble(args[2]);
		int tasksetNum_perUtil = Integer.parseInt(args[3]);
		int physicalCoreNum = 5; // 5 physical cores by default
		if(args.length >= 5){
			 physicalCoreNum = Integer.parseInt(args[4]);
		}
		
		System.out.println("Input params: tasksetUtil (" + tasksetUtil_min + "," 
					+ tasksetUtil_step + ", " + tasksetUtil_max +") tasksetNum_perUtil" + tasksetNum_perUtil + "\r\n");
		SchedulableTasksetCounter.countAverageBandwidth(tasksetUtil_min, tasksetUtil_step,
				tasksetUtil_max, tasksetNum_perUtil);
		SchedulableTasksetCounter.countEachTaskBandwidth(tasksetUtil_min, tasksetUtil_step,
				tasksetUtil_max, tasksetNum_perUtil);
		SchedulableTasksetCounter.countSchedulableTasksetRatio(tasksetUtil_min, tasksetUtil_step, tasksetUtil_max, tasksetNum_perUtil, physicalCoreNum);
		SchedulableTasksetCounter.countBandwidthSaving(tasksetUtil_min, tasksetUtil_step, tasksetUtil_max, tasksetNum_perUtil);
		SchedulableTasksetCounter.countEachTasksetBandwidthSaving(tasksetUtil_min, tasksetUtil_step, tasksetUtil_max, tasksetNum_perUtil);
	}
	
	private static void printUsage(){
		String str = "========================================\r\n";
		str += "SchedulableTasksetCounter tasksetUtil(min, step, max) tasksetNum_perUtil \r\n"; 
		str += "=========================================";
		System.out.println(str);
	}
	
	public static void countEachTasksetBandwidthSaving(double tasksetUtil_min, double tasksetUtil_step,double tasksetUtil_max,int tasksetNum_perUtil){
		DecimalFormat df = new DecimalFormat("#.##");
		df.setMinimumFractionDigits(2);
		df.setMaximumFractionDigits(2);
		int[] groupsBandwidthSaving = {GlobalVariable.COMBINED_VS_TASKCENTRIC, GlobalVariable.DMPR_VS_MPR};	

		for(int i=0; i<groupsBandwidthSaving.length; i++){
			String outputFilename = "" + df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
					+df.format(tasksetUtil_max) + "-" + tasksetNum_perUtil + "/" 
					+ df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
					+df.format(tasksetUtil_max) + "-" + tasksetNum_perUtil + "-"; 
			
			SchedulableTasksetCounter counter_TASKCENTRIC = null; //baseline
			SchedulableTasksetCounter counter_COMBINED = null; // our proposed approach to compare
			if(groupsBandwidthSaving[i] == GlobalVariable.COMBINED_VS_TASKCENTRIC){
				counter_TASKCENTRIC = new SchedulableTasksetCounter(GlobalVariable.CAMPR2hEDF_TASK_CENTRIC);
				counter_COMBINED = new SchedulableTasksetCounter(GlobalVariable.CAMPR2hEDF_COMBINED);
				outputFilename += "EachTasksetBandwidthSave-COMBINED-vs-TASKCENTRIC.stat";
			}else if(groupsBandwidthSaving[i] == GlobalVariable.DMPR_VS_MPR){
				counter_TASKCENTRIC = new SchedulableTasksetCounter(GlobalVariable.MPR2);
				counter_COMBINED = new SchedulableTasksetCounter(GlobalVariable.MPR2hEDF);
				outputFilename += "EachTasksetBandwidthSave-DMPR-vs-MPR.stat";
			}else{
				System.err.println("ERROR: Now can only compare COMBINED_VS_TASKCENTRIC and DMPR_VS_MPR");
			}
			
			counter_TASKCENTRIC.parseInterfaces(tasksetUtil_min,tasksetUtil_step,tasksetUtil_max,tasksetNum_perUtil);
			counter_COMBINED.parseInterfaces(tasksetUtil_min,tasksetUtil_step,tasksetUtil_max,tasksetNum_perUtil);
			Vector<Vector<MPRInterface>> interfaces_TASKCENTRIC =  counter_TASKCENTRIC.getmPRInterfaces();
			Vector<Vector<MPRInterface>> interfaces_COMBINED =  counter_COMBINED.getmPRInterfaces();


			try{
				BufferedWriter outputFile = new BufferedWriter(new FileWriter(outputFilename));

				String str = "";


				//Vector<Double> bandwidthSavingEachUtil = new Vector<Double>(interfaces_COMBINED.size());
				int arrayUtilIndex = 0;
				for(double util = tasksetUtil_min; util<tasksetUtil_max; util += tasksetUtil_step){
					for(int index = 0; index<tasksetNum_perUtil; index++){
						MPRInterface interface_TASKCENTRIC = interfaces_TASKCENTRIC.get(arrayUtilIndex).get(index);
						MPRInterface interface_COMBINED = interfaces_COMBINED.get(arrayUtilIndex).get(index);
						double bandwidth_TASKCENTRIC = -1;
						double bandwidth_COMBINED = -1;
						if(interface_TASKCENTRIC.getM_prime() > 0 && interface_TASKCENTRIC.getPi() > 0 
								&& interface_TASKCENTRIC.getTheta() > 0){
							bandwidth_TASKCENTRIC = interface_TASKCENTRIC.getTheta() * 1.0 / interface_TASKCENTRIC.getPi();
						}
						if(interface_COMBINED.getM_prime() > 0 && interface_COMBINED.getPi() > 0 
								&& interface_COMBINED.getTheta() > 0){
							bandwidth_COMBINED = interface_COMBINED.getTheta() * 1.0/ interface_COMBINED.getPi();
						}
						if(bandwidth_COMBINED < bandwidth_TASKCENTRIC){
							str += df.format(util) + "\t" + df.format(index)+"\t" + (bandwidth_TASKCENTRIC-bandwidth_COMBINED)+ "\r\n";
						}else{
							if(interface_TASKCENTRIC.getM_prime() == 0 && interface_TASKCENTRIC.getTheta() > 0
									&& interface_COMBINED.getM_prime() >0 && bandwidth_COMBINED > 0){
								str += df.format(util) + "\t" + df.format(index)+"\t" + bandwidth_COMBINED + "\r\n";
							}
						}

					}
					arrayUtilIndex++;
				}
				outputFile.write(str);
				outputFile.close();
				if(groupsBandwidthSaving[i] == GlobalVariable.COMBINED_VS_TASKCENTRIC){
					System.out.println("==========Each Bandwdith Save (COMBINED v.s. TASKCENTRIC)================");
				}else if(groupsBandwidthSaving[i] == GlobalVariable.DMPR_VS_MPR){
					System.out.println("==========Each Bandwdith Save (DMPR v.s. MPR)================");
				}
				System.out.println(str);

			}catch (Exception e){
				System.err.println("Open file" + outputFilename + " fails");
			}
			
		}
		
	}
	
	public static void countBandwidthSaving(double tasksetUtil_min,double tasksetUtil_step,
			double tasksetUtil_max, int tasksetNum_perUtil){
		DecimalFormat df = new DecimalFormat("#.##");
		df.setMinimumFractionDigits(2);
		df.setMaximumFractionDigits(2);
		//bandwidth saving: Overhead aware Combined vs. Task Centric; Overhead free DMPR vs. MPR 
		int[] groupsBandwidthSaving = {GlobalVariable.COMBINED_VS_TASKCENTRIC, GlobalVariable.DMPR_VS_MPR};
		
		
		
		
		for(int i=0; i<groupsBandwidthSaving.length; i++){
			String outputFilename = "" + df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
					+df.format(tasksetUtil_max) + "-" + tasksetNum_perUtil + "/" 
					+ df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
					+df.format(tasksetUtil_max) + "-" + tasksetNum_perUtil + "-"; 
			
			SchedulableTasksetCounter counter_TASKCENTRIC = null; //BASELINE APPROACH
			SchedulableTasksetCounter counter_COMBINED = null; // OUR PROPOSED APPROACH TO COMPARE
			
			if(groupsBandwidthSaving[i] == GlobalVariable.COMBINED_VS_TASKCENTRIC){
				counter_TASKCENTRIC = new SchedulableTasksetCounter(GlobalVariable.CAMPR2hEDF_TASK_CENTRIC);
				counter_COMBINED = new SchedulableTasksetCounter(GlobalVariable.CAMPR2hEDF_COMBINED);
				outputFilename += "BandwidthSaveAverage-COMBINED-vs-TASKCENTRIC.stat";
			}else if(groupsBandwidthSaving[i] == GlobalVariable.DMPR_VS_MPR){
				counter_TASKCENTRIC = new SchedulableTasksetCounter(GlobalVariable.MPR2);
				counter_COMBINED = new SchedulableTasksetCounter(GlobalVariable.MPR2hEDF);
				outputFilename += "BandwidthSaveAverage-DMPR-vs-MPR.stat";
			}else{
				System.err.println("ERROR: Now can only compare COMBINED_VS_TASKCENTRIC and DMPR_VS_MPR");
			}
			 
			counter_TASKCENTRIC.parseInterfaces(tasksetUtil_min,tasksetUtil_step,tasksetUtil_max,tasksetNum_perUtil);
			counter_COMBINED.parseInterfaces(tasksetUtil_min,tasksetUtil_step,tasksetUtil_max,tasksetNum_perUtil);
			Vector<Vector<MPRInterface>> interfaces_TASKCENTRIC =  counter_TASKCENTRIC.getmPRInterfaces();
			Vector<Vector<MPRInterface>> interfaces_COMBINED =  counter_COMBINED.getmPRInterfaces();
		
			try{
				BufferedWriter outputFile = new BufferedWriter(new FileWriter(outputFilename));

				String str = "";

				double numTasksetBandwidthSave = 0;
				double numTasksetTotalTaskset = 0;
				//Vector<Double> bandwidthSavingEachUtil = new Vector<Double>(interfaces_COMBINED.size());
				int arrayUtilIndex = 0;
				for(double util = tasksetUtil_min; util<tasksetUtil_max; util += tasksetUtil_step){
					double bandwidthTotalSavePerUtil = 0;
					double numTasksetBandwidthSavePerUtil = 0;
					for(int index = 0; index<tasksetNum_perUtil; index++){
						MPRInterface interface_TASKCENTRIC = interfaces_TASKCENTRIC.get(arrayUtilIndex).get(index);
						MPRInterface interface_COMBINED = interfaces_COMBINED.get(arrayUtilIndex).get(index);
						double bandwidth_TASKCENTRIC = -1;
						double bandwidth_COMBINED = -1;
						if(interface_TASKCENTRIC.getM_prime() > 0 && interface_TASKCENTRIC.getPi() > 0 
								&& interface_TASKCENTRIC.getTheta() > 0){
							bandwidth_TASKCENTRIC = interface_TASKCENTRIC.getTheta() * 1.0 / interface_TASKCENTRIC.getPi();
						}
						if(interface_COMBINED.getM_prime() > 0 && interface_COMBINED.getPi() > 0 
								&& interface_COMBINED.getTheta() > 0){
							bandwidth_COMBINED = interface_COMBINED.getTheta() * 1.0/ interface_COMBINED.getPi();
						}
						if(bandwidth_COMBINED < bandwidth_TASKCENTRIC){
							bandwidthTotalSavePerUtil += bandwidth_TASKCENTRIC - bandwidth_COMBINED;
							numTasksetBandwidthSavePerUtil++;
							numTasksetBandwidthSave++;
						}else{
							if(interface_TASKCENTRIC.getM_prime()==0 && interface_TASKCENTRIC.getTheta()>0 
									&& interface_COMBINED.getM_prime() != 0 && bandwidth_COMBINED != 0 ){
								bandwidthTotalSavePerUtil += bandwidth_COMBINED;
								numTasksetBandwidthSavePerUtil++;
								numTasksetBandwidthSave++;
							}
						}
						numTasksetTotalTaskset++;
					}
					if(numTasksetBandwidthSavePerUtil != 0){
						//str += df.format(util) + "\t" + df.format(bandwidthTotalSavePerUtil/numTasksetBandwidthSavePerUtil) + "\r\n";
						str += df.format(util) + "\t" + df.format(bandwidthTotalSavePerUtil) + "\r\n";
					}else{
						str += df.format(util) + "\t" + df.format(0.00) + "\r\n";
					}
					arrayUtilIndex++;
				}

				if(numTasksetTotalTaskset != 0){
					str += df.format(-1) + "\t" + df.format(numTasksetBandwidthSave*1.0/numTasksetTotalTaskset);
				}
				outputFile.write(str);
				outputFile.close();
				if(groupsBandwidthSaving[i] == GlobalVariable.COMBINED_VS_TASKCENTRIC){
					System.out.println("==========Average Bandwdith Save (COMBINED v.s. TASKCENTRIC)================");
				}else if(groupsBandwidthSaving[i] == GlobalVariable.DMPR_VS_MPR){
					System.out.println("==========Average Bandwdith Save (DMPR v.s. MPR)================");
				}
				System.out.println(str);
				System.out.println("Ratio of task set that COMBINED/DMPR saves bandwidth than TASKCENTRIC/DMPR: " + df.format(numTasksetBandwidthSave*1.0/numTasksetTotalTaskset));

			}catch (Exception e){
				System.err.println("Open file" + outputFilename + " fails");
			}
		}
		
	}
	
	/**
	 * Count the ratio of schedulable task set
	 * @param tasksetUtil_min
	 * @param tasksetUtil_step
	 * @param tasksetUtil_max
	 * @param tasksetNum_perUtil
	 * @param physicalCoreNum
	 */
	public static void countSchedulableTasksetRatio(double tasksetUtil_min,double tasksetUtil_step,
			double tasksetUtil_max, int tasksetNum_perUtil, int physicalCoreNum){
		
		for(int i=0; i<whichApproaches.length; i++){
			int whichApproach = whichApproaches[i];
			SchedulableTasksetCounter counter = new SchedulableTasksetCounter(whichApproach);
			counter.parseInterfaces(tasksetUtil_min,tasksetUtil_step,tasksetUtil_max,tasksetNum_perUtil);
			counter.writeSchedulableTasksetRatioPerUtil(tasksetUtil_min, tasksetUtil_step, tasksetUtil_max, tasksetNum_perUtil, physicalCoreNum);
		}
		
	}
	
	
	/**
	 * Function countAverageBandwidth
	 * Count the average bandwdith for each task set utilization for each approach.
	 * Draw each line for each model: the x-axis is the taskset utilizaiton,
	 * the y-axis is the average bandwidth of the tasksets with such utilization
	 */
	public static void countAverageBandwidth(double tasksetUtil_min, double tasksetUtil_step,
			double tasksetUtil_max, int tasksetNum_perUtil){
		
		//When we add more calculation approach, we can just add it to the whichApproaches array, add the approch to the swtich(), and done!
		
		for(int i=0; i<whichApproaches.length; i++){
			int whichApproach = whichApproaches[i];
			SchedulableTasksetCounter counter = new SchedulableTasksetCounter(whichApproach);
			counter.parseInterfaces(tasksetUtil_min,tasksetUtil_step,tasksetUtil_max,tasksetNum_perUtil);
			counter.writeAverageBandwidthPerUtil(tasksetUtil_min, tasksetUtil_step, tasksetUtil_max, tasksetNum_perUtil);
		}
		
	}
	
	/**
	 * Function countEachTaskBandwidth
	 * Count the interface's bandwidth for each task in each task set utilization.
	 * The x-axis is the taskset's index, e.g. 0.10-0
	 * The y-axis is the system's bandwidth of this taskset.
	 */
	public static void countEachTaskBandwidth(double tasksetUtil_min, double tasksetUtil_step,
			double tasksetUtil_max, int tasksetNum_perUtil){
		//When we add more calculation approach, we can just add it to the whichApproaches array, add the approch to the swtich(), and done!
		
		for(int i=0; i<whichApproaches.length; i++){
			int whichApproach = whichApproaches[i];
			SchedulableTasksetCounter counter = new SchedulableTasksetCounter(whichApproach);
			counter.parseInterfaces(tasksetUtil_min,tasksetUtil_step,tasksetUtil_max,tasksetNum_perUtil);
			counter.writeEachTasksetBandwidth(tasksetUtil_min, tasksetUtil_step, tasksetUtil_max, tasksetNum_perUtil);
		}
	}
	
	public void writeEachTasksetBandwidth(double tasksetUtil_min,double tasksetUtil_step, double tasksetUtil_max, int tasksetNum_perUtil){
		DecimalFormat df = new DecimalFormat("#.##");
		df.setMinimumFractionDigits(2);
		df.setMaximumFractionDigits(2);
		String outputFilename = "" + df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
				+df.format(tasksetUtil_max) + "-" + tasksetNum_perUtil + "/" 
				+ df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
				+df.format(tasksetUtil_max) + "-" + tasksetNum_perUtil + "-";
		switch(this.whichApproach){
		case GlobalVariable.CAMPR2hEDF_TASK_CENTRIC: outputFilename += "CAMPR2hEDF_TASKCENTRIC-EachTasksetBandwidth.stat"; break;
		case GlobalVariable.CAMPR2hEDF_MODEL_CENTRIC: outputFilename += "CAMPR2hEDF_MODELCENTRIC-EachTasksetBandwidth.stat"; break;
		case GlobalVariable.CAMPR2hEDF_COMBINED: outputFilename += "CAMPR2hEDF_COMBINED-EachTasksetBandwidth.stat"; break;
		case GlobalVariable.MPR2: outputFilename += "MPR2-EachTasksetBandwidth.stat"; break;
		case GlobalVariable.MPR2hEDF: outputFilename += "MPR2hEDF-EachTasksetBandwidth.stat"; break;
		default: System.err.println("SchedulableTasksetCounter: No such computation approach! exit"); System.exit(1);
		}
		
		try{
			BufferedWriter outputFile = new BufferedWriter(new FileWriter(outputFilename));
			String str = "";
			int arrayUtilIndex = 0;
			for(double util = tasksetUtil_min; util<tasksetUtil_max; util += tasksetUtil_step){
				for(int index = 0; index<tasksetNum_perUtil; index++){
					MPRInterface currentInterface = this.mPRInterfaces.get(arrayUtilIndex).get(index);
					if(currentInterface.getTheta() > 0 
							&& currentInterface.getTheta() <= currentInterface.getM_prime() * currentInterface.getPi()){//only count the feasible ones. Because infeasible one's bandwidth has no meaning
						//tasksetindex(0.10-0) averageBandwidth
						str += df.format(util) + "\t" + index + "\t" + df.format(currentInterface.getTheta()/currentInterface.getPi()) + "\r\n";
					}else if(currentInterface.getTheta() > 0 
							&& currentInterface.getTheta() > currentInterface.getM_prime() * currentInterface.getPi()){
						str += df.format(util) + "\t" + index + "\t" + df.format(GlobalVariable.INTERFACE_INFEASIBLE) + "\r\n";
					}else if(currentInterface.getTheta() <= 0 && currentInterface.getTheta() == GlobalVariable.FILE_NOT_EXIST){
						System.err.println("This interface file does not exist!");
					}else if(currentInterface.getTheta() == 0 && currentInterface.getM_prime() == 0){
						System.err.println("This interface's Theta = 0, taskset is empty");
					}else{
						System.err.println("Theta is negative and it's not because the interface file not exists. Check the input file: taskset util=" + util + "\ttaskset index=" + index + "\r\n");
						System.exit(1);
					}
				}
				arrayUtilIndex++;
			}
			System.out.println("write averate bandwidth of each taskset utilization to the file "+outputFilename);
			System.out.println("========="+ outputFilename + "===============");
			outputFile.write(str);
			outputFile.close();
			
		}catch (Exception e){
			System.err.println("Exception in writerAverageBandwidthPerUtil: " + e.getMessage() + "\texit.");
			e.printStackTrace();
			System.exit(1);
		}
	}
	
	/**
	 * Function writeAverageBandwidthPerUtil
	 * Write the average bandwidth of a taskset utilization v.s. taskset utilization into the file.
	 * The taskset utilization is given by the function's parameter.
	 * @param tasksetUtil_min is the minimum taskset utilization
	 * @param tasksetUtil_step is the step by which the taskset utilization increases
	 * @param tasksetUtil_max is the maximum taskset utilization
	 * @param tasksetNum_perUtil is the number of tasksets for each taskset utilization
	 */
	public void writeAverageBandwidthPerUtil(double tasksetUtil_min, double tasksetUtil_step, double tasksetUtil_max, int tasksetNum_perUtil){
		DecimalFormat df = new DecimalFormat("#.##");
		df.setMinimumFractionDigits(2);
		df.setMaximumFractionDigits(2);
		String outputFilename = "" + df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
				+df.format(tasksetUtil_max) + "-" + (int)tasksetNum_perUtil + "/" 
				+ df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
				+df.format(tasksetUtil_max) + "-" + (int)tasksetNum_perUtil + "-";
		switch(this.whichApproach){
		case GlobalVariable.CAMPR2hEDF_TASK_CENTRIC: outputFilename += "CAMPR2hEDF_TASKCENTRIC-AverageBandwidth.stat"; break;
		case GlobalVariable.CAMPR2hEDF_MODEL_CENTRIC: outputFilename += "CAMPR2hEDF_MODELCENTRIC-AverageBandwidth.stat"; break;
		case GlobalVariable.CAMPR2hEDF_COMBINED: outputFilename += "CAMPR2hEDF_COMBINED-AverageBandwidth.stat"; break;
		case GlobalVariable.MPR2: outputFilename += "MPR2-AverageBandwidth.stat"; break;
		case GlobalVariable.MPR2hEDF: outputFilename += "MPR2hEDF-AverageBandwidth.stat"; break;
		default: System.err.println("SchedulableTasksetCounter: No such computation approach! exit"); System.exit(1);
		}
		
		try{
			BufferedWriter outputFile = new BufferedWriter(new FileWriter(outputFilename));
			String str = "";
			int arrayUtilIndex = 0;
			for(double util = tasksetUtil_min; util<tasksetUtil_max; util += tasksetUtil_step){
				double bandwidthTotal_perUtil = 0;
				double tasksetTotal_perUtil = 0;
				for(int index = 0; index<tasksetNum_perUtil; index++){
					MPRInterface currentInterface = this.mPRInterfaces.get(arrayUtilIndex).get(index);
					if( currentInterface.getTheta() > 0 
							&& currentInterface.getTheta() <= currentInterface.getM_prime() * currentInterface.getPi()){//only count the feasible ones. Because infeasible one's bandwidth has no meaning
						bandwidthTotal_perUtil += currentInterface.getTheta() / currentInterface.getPi();
						tasksetTotal_perUtil++;
					}
				}
				//tasksetUtil averageBandwidth
				if(tasksetTotal_perUtil != 0){
					str += df.format(util) + "\t" + df.format(bandwidthTotal_perUtil/tasksetTotal_perUtil) + "\r\n";
				}
				arrayUtilIndex++;
			}
			System.out.println("write averate bandwidth of each taskset utilization to the file "+outputFilename);
			outputFile.write(str);
			System.out.println("========="+ outputFilename + "===============");
			System.out.println(str);
			outputFile.close();
			
		}catch (Exception e){
			System.err.println("Exception in writerAverageBandwidthPerUtil: " + e.getMessage() + "\texit.");
			e.printStackTrace();
			System.exit(1);
		}
	}
	
	public void writeSchedulableTasksetRatioPerUtil(double tasksetUtil_min, double tasksetUtil_step, 
			double tasksetUtil_max, double tasksetNum_perUtil, int physicalCoreNum){
		DecimalFormat df = new DecimalFormat("#.##");
		df.setMinimumFractionDigits(2);
		df.setMaximumFractionDigits(2);
		String outputFilename = "" + df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
				+df.format(tasksetUtil_max) + "-" + (int)tasksetNum_perUtil + "/" 
				+ df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
				+df.format(tasksetUtil_max) + "-" + (int)tasksetNum_perUtil + "-";
		switch(this.whichApproach){
		case GlobalVariable.CAMPR2hEDF_TASK_CENTRIC: outputFilename += "CAMPR2hEDF_TASKCENTRIC-SchedulableTasksetRatio.stat"; break;
		case GlobalVariable.CAMPR2hEDF_MODEL_CENTRIC: outputFilename += "CAMPR2hEDF_MODELCENTRIC-SchedulableTasksetRatio.stat"; break;
		case GlobalVariable.CAMPR2hEDF_COMBINED: outputFilename += "CAMPR2hEDF_COMBINED-SchedulableTasksetRatio.stat"; break;
		case GlobalVariable.MPR2: outputFilename += "MPR2-SchedulableTasksetRatio.stat"; break;
		case GlobalVariable.MPR2hEDF: outputFilename += "MPR2hEDF-SchedulableTasksetRatio.stat"; break;
		default: System.err.println("in writeSchedulableTasksetRatioPerUtil() of SchedulableTasksetCounter: No such computation approach! exit"); System.exit(1);
		}
		
		try{
			BufferedWriter outputFile = new BufferedWriter(new FileWriter(outputFilename));
			String str = "";
			int arrayUtilIndex = 0;
			for(double util = tasksetUtil_min; util<tasksetUtil_max; util += tasksetUtil_step){
				double schedulableTasksetTotal_perUtil = 0;
				double tasksetTotal_perUtil = 0;
				for(int index = 0; index<tasksetNum_perUtil; index++){
					MPRInterface currentInterface = this.mPRInterfaces.get(arrayUtilIndex).get(index);
					if( currentInterface.getTheta() > 0 
							&& currentInterface.getTheta() <= currentInterface.getM_prime() * currentInterface.getPi()
							&& currentInterface.getM_prime() <= physicalCoreNum){//only count the feasible ones. Because infeasible one's bandwidth has no meaning
						schedulableTasksetTotal_perUtil++;
					}
					if(currentInterface.getTheta() != GlobalVariable.FILE_NOT_EXIST 
							&& currentInterface.getTheta() != GlobalVariable.TASKSET_EMPTY){ //handle when filter out some task set in ARVIND_SCHEDTEST
						tasksetTotal_perUtil++;
					}
					
				}
				//tasksetUtil averageBandwidth
				if(tasksetTotal_perUtil != 0){ 
					str += df.format(util) + "\t" + df.format(schedulableTasksetTotal_perUtil/tasksetTotal_perUtil) + "\r\n";
				}
				arrayUtilIndex++;
			}
			System.out.println("write schedulable taskset ratio for each taskset utilization to the file "+outputFilename);
			outputFile.write(str);
			System.out.println("========="+ outputFilename + "===============");
			System.out.println(str);
			outputFile.close();
			
		}catch (Exception e){
			System.err.println("Exception in writerAverageBandwidthPerUtil: " + e.getMessage() + "\texit.");
			e.printStackTrace();
			System.exit(1);
		}
	}
	
	/**
	 * Function parseInterface
	 * It parse each output xml file, which has the calculated interface, and get the MPR interface for each xml file.
	 * Save the interfaces into the vector as the object's property
	 */
	public void parseInterfaces(double tasksetUtil_min, double tasksetUtil_step, double tasksetUtil_max, int tasksetNum_perUtil){
		DecimalFormat df = new DecimalFormat("#.##");
		df.setMinimumFractionDigits(2);
		df.setMaximumFractionDigits(2);
		int arrayUtilIndex = 0;
		for(double util = tasksetUtil_min; util<tasksetUtil_max; util+=tasksetUtil_step){
			Vector<MPRInterface> mPRInterfaces_util = new Vector<MPRInterface>();
			for(int tasksetIndex =0; tasksetIndex < tasksetNum_perUtil; tasksetIndex++){
				String inputFilename = "" + df.format(tasksetUtil_min) + "-" + df.format(tasksetUtil_step) + "-" 
						+df.format(tasksetUtil_max) + "-" + tasksetNum_perUtil + "/" 
						+ df.format(util) + "/"
						+ tasksetIndex + "-" +  df.format(util) + "-";
				switch(this.whichApproach){
				case GlobalVariable.CAMPR2hEDF_TASK_CENTRIC: inputFilename += "CAMPR2hEDF_TASKCENTRIC-out.xml"; break;
				case GlobalVariable.CAMPR2hEDF_MODEL_CENTRIC: inputFilename += "CAMPR2hEDF_MODELCENTRIC-out.xml"; break;
				case GlobalVariable.CAMPR2hEDF_COMBINED: inputFilename += "CAMPR2hEDF_COMBINED-out.xml"; break;
				case GlobalVariable.MPR2: inputFilename += "MPR2-out.xml"; break;
				case GlobalVariable.MPR2hEDF: inputFilename += "MPR2hEDF-out.xml"; break;
				default: System.err.println("SchedulableTasksetCounter: No such computation approach! exit"); System.exit(1);
				}
				MPRInterface mPRInterface = this.getRootComponentInterface(inputFilename);
				mPRInterfaces_util.add(mPRInterface);
			}
			
			if(this.mPRInterfaces == null){
				this.mPRInterfaces = new Vector<Vector<MPRInterface>>();
			}else{
				this.mPRInterfaces.add(mPRInterfaces_util);
			}
			arrayUtilIndex++;
		}
		
		if(arrayUtilIndex != this.mPRInterfaces.size()){
			System.err.println("The interfaces vector has incorrect dimension in parser. exit(1)");
			System.exit(1);
		}
		
	}
	
	/**
	 * Function SchedulableTasksetCounter
	 * Construct function
	 * @param whichApproach is defined in the GlobalVariable class in the same package
	 */
	public SchedulableTasksetCounter(int whichApproach){
		this.whichApproach = whichApproach;
		this.mPRInterfaces = new Vector<Vector<MPRInterface>>();
	}

	public MPRInterface getRootComponentInterface(String inputFilename){
		try{
			File inputFile_temp = new File(inputFilename);
			if(!inputFile_temp.exists()){
				System.err.println("File " + inputFilename + " not exist.");
				return new MPRInterface(GlobalVariable.FILE_NOT_EXIST, GlobalVariable.FILE_NOT_EXIST, GlobalVariable.FILE_NOT_EXIST);
			}
			
			BufferedReader inputFile = new BufferedReader(new FileReader(inputFilename));
			String currentLine = "";
			
			
			while( (currentLine = inputFile.readLine()) != null){
				
				if(currentLine.indexOf("<model") != -1){
					String[] subStrs = currentLine.split("\"");
					int m_prime = Integer.parseInt(subStrs[1]);
					double Pi = Double.parseDouble(subStrs[3]);
					double Theta = Double.parseDouble(subStrs[5]);
					Tool.debug("m'=" + m_prime + ", Pi=" + Pi + ", Theta=" + Theta + "\r\n");
					MPRInterface mPRInterface = new MPRInterface(Pi, Theta, m_prime);
					inputFile.close();
					return mPRInterface;
					
				}
			}
			System.err.println("Parse inputfile:" + inputFilename + "Error! No top component's interface");
			inputFile.close();
			
		}catch (Exception e){
			System.err.println("isSchedulableTaskset() excetion:" + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
		
		return  new MPRInterface(GlobalVariable.FILE_NOT_EXIST, GlobalVariable.FILE_NOT_EXIST, GlobalVariable.FILE_NOT_EXIST);
		
	}

	public Vector<Vector<MPRInterface>> getmPRInterfaces() {
		return mPRInterfaces;
	}

	public void setmPRInterfaces(Vector<Vector<MPRInterface>> mPRInterfaces) {
		this.mPRInterfaces = mPRInterfaces;
	}
	
	
	
}
