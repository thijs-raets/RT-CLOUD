package edu.penn.rtg.experiment.tasksetgenerator;

public class GlobalVariable {
	public static final double TIME_PRECISION = 1;
	public static final long MAX_NUMBER = 2047483647; //9223372036854775807;//largest number of long type in java. 32 bit
	public static final double ONE_TIME_UNIT = Math.pow(0.1,6);
	
	public static final int CAMPR2hEDF_TASK_CENTRIC = 1;
	public static final int CAMPR2hEDF_MODEL_CENTRIC = 2;
	public static final int MPR2 = 3;
	public static final int MPR2hEDF = 4;
	public static final int CAMPR2hEDF_COMBINED = 5; 
	
	public static final int FILE_NOT_EXIST = -10;
	public static final int INTERFACE_INFEASIBLE = -1;
	public static final int TASKSET_EMPTY = 0;
	
	public static final int COMBINED_VS_TASKCENTRIC = 1;
	public static final int DMPR_VS_MPR = 2;
	
	public static final int UNIFROM_LIGHT = 1;
	public static final int UNIFROM_MEDIUM = 2;
	public static final int UNIFROM_HEAVY = 3;
	public static final int BIMODAL_LIGHT = 4;
	public static final int BIMODAL_MEDIUM = 5;
	public static final int BIMODAL_HEAVY = 6;
	
	@Override
	public String toString() {
		return "GlobalVariable [TIME_PRECISION=" + TIME_PRECISION + "]";
	}
	
}
