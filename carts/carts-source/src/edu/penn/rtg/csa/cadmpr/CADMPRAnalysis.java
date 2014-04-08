package edu.penn.rtg.csa.cadmpr;

import edu.penn.rtg.common.*;

/**
 * Class CAMPR2hEDFAnalysis
 * It's the start point of the cache aware MPR analysis
 * It parse the input xml file, call the Component.java to analysis the system recursively and 
 * output the result to output xml file.
 * 
 * JDK version used: <JDK1.7>
 * @author Meng Xu
 * Create Date: 4/6/2013 
 *
 */
public class CADMPRAnalysis {
	public static void process(String[] args){
		if(args.length < 3){
			error();
		}
		
		try{
			int whichSchedTest = -1;
			if(args.length == 3){
				//DEFAULT SCHED TEST!
				whichSchedTest = GlobalVariable.ARVIND_SCHEDTEST; //ARVIND_SCHEDTEST;//BERTOGNA_SCHEDTEST
				
			}else if(args.length >= 4){ // >= to make it easier in eclipse to debug.
				whichSchedTest = parseWhichSchedTest(args[3]);
				
			}
			System.out.println("Command is:" + args[0] + " " + args[1] + " " + args[2] + " whichSchedTest="+ whichSchedTest);
			
			if(args[1].equalsIgnoreCase("CAMPR2hEDF_TASKCENTRIC") || args[1].equalsIgnoreCase("CADMPR_TASKCENTRIC") ){
				System.out.println("Using Task Centric overhead accounting technique");
				run(args[0],args[1],args[2],GlobalVariable.TASK_CENTRIC, whichSchedTest);
			}else if(args[1].equalsIgnoreCase("CAMPR2hEDF_MODELCENTRIC") || args[1].equalsIgnoreCase("CADMPR_MODELCENTRIC")){
				System.out.println("Using Model Centric overhead accounting technique");
				run(args[0],args[1],args[2],GlobalVariable.MODEL_CENTRIC, whichSchedTest);
			}else if(args[1].equalsIgnoreCase("CADMPR_HYBRID") || args[1].equalsIgnoreCase("CAMPR2hEDF_HYBRID")){
				run(args[0],args[1],args[2],GlobalVariable.HYBRID, whichSchedTest);
			}else{
				System.out.println("No such analysis approach, please check the analysis approach: " + args[1] + "\r\n exit(1)");
				System.exit(1);
			}
			
		}catch(Exception e){
			e.printStackTrace();
			System.err.println("Exception trown out in the analysis");
		}
	}
	
	public static void run(String inputFilename, String resourceModel, String outputFilename,int whichApproach, int whichSchedTest ){

		XMLInterpreter4CADMPR xmlInterpreter = new XMLInterpreter4CADMPR(inputFilename); //4 is short for "for"
		
		xmlInterpreter.parseFile();
		Component rootComponent = xmlInterpreter.getRootComponent();
		
		//reset the crpmd = 1.9ms which is the crpmd cost when task set WSS is 256KB. Only to walk around when delta_crpmd is wrongly set in input xml
		//rootComponent.resetDelta_crpmd();
		
		
		switch(whichApproach){
		case GlobalVariable.TASK_CENTRIC: doCADMPRTaskCentricAnalysis(rootComponent, whichApproach, whichSchedTest); break;
		case GlobalVariable.MODEL_CENTRIC: doCADMPRModelCentricAnalysis(rootComponent, whichApproach, whichSchedTest); break;
		case GlobalVariable.HYBRID: rootComponent = doCADMPRHybridAnalysis(rootComponent, whichApproach, whichSchedTest); break;
		default: System.err.println("No such overhead aware MPR analysis approach. Only suport TaskCentri and ModelCentric approach now. Exit(1)"); System.exit(1);
		}
				
		XMLWriter4CADMPR xmlWriter = new XMLWriter4CADMPR(outputFilename, rootComponent);
		
		xmlWriter.writeComponentInterfaceTree(whichApproach);
		
	}
	
	public static int parseWhichSchedTest(String str){
		if(str.equalsIgnoreCase("ARVIND_SCHEDTEST")) return GlobalVariable.ARVIND_SCHEDTEST;
		if(str.equalsIgnoreCase("BERTOGNA_SCHEDTEST")) return GlobalVariable.BERTOGNA_SCHEDTEST;
		if(str.equalsIgnoreCase("ARVIND_SCHEDTEST_FAST")) return GlobalVariable.ARVIND_SCHEDTEST_FAST;
		if(str.equalsIgnoreCase("MENG_SCHEDTEST")) return GlobalVariable.MENG_SCHEDTEST;
		
		System.err.println("In parseWhichSchedTest() No such sched test! System exit.");
		System.exit(1);
		return 0;
	}
	
	private static void error() {
		System.err.println("Invalid arguments");
		System.err
				.println("Carts <input_XML_file> <scheduling_algo> <output_XML_file>");
		System.err
		.println("<scheduling_algo> can be : PRM / EDP / EQV / MPR (MPR2) / DMPR / " +
				"CADMPR_TASKCENTRIC (CAMPR2hEDF_TASKCENTRIC) / CADMPR_MODELCENTRIC (CAMPR2hEDF_MODELCENTRIC) / " +
				"CADMPR_HYBRID (CAMPR2hEDF_HYBRID)");
		System.exit(1);
		
	}
	
	public static String doCADMPRTaskCentricAnalysis(Component rootComponent, int whichApproach, int whichSchedTest){
		String result = checkSchedulingAlgorithm(rootComponent);
		rootComponent.inflateTaskWCET_taskCentric();
		rootComponent.doCSA(whichSchedTest, whichApproach);
		return result;
	}
	
	public static String doCADMPRModelCentricAnalysis(Component rootComponent, int whichApproach, int whichSchedTest){
		String result = checkSchedulingAlgorithm(rootComponent);
		rootComponent.inflateTaskWCETEV1_Bjorn();
		rootComponent.doCSA(whichSchedTest, whichApproach);
		return result;
	}
	
	public static Component doCADMPRHybridAnalysis(Component rootComponent, int whichApproach, int whichSchedTest){
		//check scheduling algorithm, which must be hybrid EDF(i.e., gEDF, hEDF, EDF in input xml)
		String result_hybrid = checkSchedulingAlgorithm(rootComponent);
		if(!result_hybrid.contains("SUCCESS")){
			rootComponent.setInterfaceComputed(false);
			return rootComponent;
		}
		
		Component component_taskcentric = new Component(rootComponent, null);	
		String result_taskcentric = doCADMPRTaskCentricAnalysis(component_taskcentric,GlobalVariable.TASK_CENTRIC, whichSchedTest );
	
		Component component_modelcentric = new Component(rootComponent, null);
		String result_modelcentric = doCADMPRModelCentricAnalysis(component_modelcentric, GlobalVariable.MODEL_CENTRIC, whichSchedTest);
		
		CADMPR cadmpr_taskcentric = component_taskcentric.getCacheAwareMPRInterface();
		CADMPR cadmpr_modelcentric = component_modelcentric.getCacheAwareMPRInterface();
		double totalBudget_taskcentric = cadmpr_taskcentric.getPi()*cadmpr_taskcentric.getM_dedicatedCores() 
							+ cadmpr_taskcentric.getTheta();
		double totalBudget_modelcentric = cadmpr_modelcentric.getPi()*cadmpr_modelcentric.getM_dedicatedCores()
							+ cadmpr_modelcentric.getTheta();
		
		//return the root component which has the result of Hybrid approach
		if(totalBudget_taskcentric <= totalBudget_modelcentric){
			return new Component(component_taskcentric,null);
		}else{
			return new Component(component_modelcentric, null);
		}

	}
	
	public static String checkSchedulingAlgorithm(Component rootComponent){
		String result = "";
		if (!(rootComponent.getSchedulingPolicy().equalsIgnoreCase("hEDF")
				|| rootComponent.getSchedulingPolicy().equalsIgnoreCase("EDF") || rootComponent
				.getSchedulingPolicy().equalsIgnoreCase("gEDF"))) {
			return "ERROR: only support hEDF scheduling policy in each component with CADMPR model";
		}
		for (Component component : rootComponent.getChildComponents()) {
			// EDF is considered as gEDF by default
			String child_result = checkSchedulingAlgorithm(component);
			if(!child_result.contains("SUCCESS")){
				return child_result;
			}
			
		}

		return "SUCCESS!";
	}

}
