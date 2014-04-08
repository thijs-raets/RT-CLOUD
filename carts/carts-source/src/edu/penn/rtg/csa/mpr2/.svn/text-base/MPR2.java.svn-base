package edu.penn.rtg.csa.mpr2;

import java.util.Vector;

/**
 * Class MPR2.
 * It's a POJO. It is an interface of a component, which uses the MPR model proposed by Insik and Arvind
 * 
 * JDK version used: <JDK1.7>
 * @author Meng Xu
 * Create Date: 4/4/2013 
 *
 */
public class MPR2 {
	private double Pi; 		// period
	private double Theta;	// execution time
	private  int m_prime;	// number of VCPU
	
	public double getSBF(double t){
		double t_prime = t - (Pi - Math.ceil(Theta/m_prime));
		double x = t_prime - Pi*Math.floor(t_prime/Pi);
		double y = Pi - Math.floor(Theta/m_prime);
		double beta = Theta - m_prime * Math.floor(Theta/m_prime);
		if(t_prime < 0){
			return 0;
		}else if(t_prime >= 0 && (x >= 1 && x<= y)){
			return Math.floor(t_prime/Pi)*Theta + Math.max(0, m_prime*x - (m_prime*Pi - Theta));
		}else if(t_prime >= 0 && (x < 1 || x > y)){
			return Math.floor(t_prime/Pi)*Theta + Math.max(0, m_prime*x - (m_prime*Pi - Theta)) - (m_prime - beta);
		}else{
			System.err.println("ERROR: CacheAwareMPR getSBF(t) is wrong! exit(1).");
			System.exit(1);
		}
		return -1;	// correct behavior won't execute this line.
	}
	
	
	
	public MPR2() {
		
		this.Pi = -1;
		this.Theta = -1;
		this.m_prime = 0;
	}
	
	public MPR2(double Pi, double Theta, int m_prime) {
		this.Pi = Pi;
		this.Theta = Theta;
		this.m_prime = m_prime;
	}

	public String toString(){
		return "(" + this.Pi + "," +this.Theta + "," + this.m_prime + ")";
	}

	public double getPi() {
		return Pi;
	}
	public void setPi(double pi) {
		Pi = pi;
	}
	public double getTheta() {
		return Theta;
	}
	public void setTheta(double theta) {
		Theta = theta;
	}
	public int getM_prime() {
		return m_prime;
	}

	public void setM_prime(int m_prime) {
		this.m_prime = m_prime;
	}

	
}
