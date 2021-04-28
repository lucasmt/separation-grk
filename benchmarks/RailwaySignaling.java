package benchmark;

import static java.nio.charset.StandardCharsets.UTF_8;
import static java.nio.file.StandardOpenOption.APPEND;
import static java.nio.file.StandardOpenOption.CREATE;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;

//import com.sun.org.apache.xml.internal.serialize.LineSeparator;

public class RailwaySignaling {

	public static void main(String[] args) throws Exception {

		if (args.length != 2) {
			System.out.println("Please provide two integers, n and k, such that 0<=k<n");
			return;
		}

		// extract the parameters and define the spec name

		int delay = Integer.parseInt(args[0]);
		int railways = Integer.parseInt(args[1]);
		String family = "railway_signaling_" + delay;
		String filename = family + "/" + family + "_" + railways + ".sgrk";

		// calculate #bits to encode the delay in binary
		int varnum = 0;
		while (Math.pow(2, varnum) <= delay)
			varnum++;

		// create initial constraints
		String initA = "";
		String initB = "";

		for (int i = 1; i <= railways; i++) {
			String toAddA0 = "(!\"in:light" + i + "\" -> (" + num2asrt(1, varnum, i, "in") + "))";
			String toAddA1 = "(\"in:light" + i + "\" -> (" + num2asrt(0, varnum, i, "in") + "))";
			String toAddB0 = "(!\"out:light" + i + "\" -> (" + num2asrt(1, varnum, i, "out") + "))";
			String toAddB1 = "(\"out:light" + i + "\" -> (" + num2asrt(0, varnum, i, "out") + "))";
			
			initA = initA +  toAddA0 + " &" + System.lineSeparator();
			initA = initA +  toAddA1 + " &" + System.lineSeparator();
			
			initB = initB +  toAddB0 + " &" + System.lineSeparator();
			initB = initB +  toAddB1 + " &" + System.lineSeparator();
		}
		
		initA.trim();
		initA = initA.replaceAll("\\s+$", "");
		initA = initA.substring(0, initA.length() - 1);
		initB.trim();
		initB = initB.replaceAll("\\s+$", "");
		initB = initB.substring(0, initB.length() - 1);
		System.out.println();
		System.out.println("system A initial constraints:" + System.lineSeparator() + initA + System.lineSeparator()
				+ System.lineSeparator() + System.lineSeparator());
		System.out.println("system B initial constraints:" + System.lineSeparator() + initB + System.lineSeparator()
				+ System.lineSeparator() + System.lineSeparator());

		// create system safety constraints
		String safeA = "";
		String safeB = "";
		// for systemA: no two traffic lights are on simultaneously
		for (int i = 1; i < railways; i++) {
			for (int j = i + 1; j <= railways; j++) {
				safeA = safeA + "(!\"in:light" + i + "\" | !\"in:light" + j + "\") & ";
			}
			safeA = safeA + System.lineSeparator();
		}
		// For system B: for m=0.. n/2, no two traffic lights in range 2m+1..2m+4 are on
		// simultaneously
		for (int i = 1; i < railways; i = i + 2) {

			if (i + 1 <= railways) {
				safeB = safeB + "(!\"out:light" + i + "\" | !\"out:light" + (i + 1) + "\") & ";
			}
			if (i + 2 <= railways) {
				safeB = safeB + "(!\"out:light" + i + "\" | !\"out:light" + (i + 2) + "\") & ";
				safeB = safeB + "(!\"out:light" + (i + 1) + "\" | !\"out:light" + (i + 2) + "\") & ";
			}
			if (i + 3 <= railways) {
				safeB = safeB + "(!\"out:light" + i + "\" | !\"out:light" + (i + 3) + "\") & ";
				safeB = safeB + "(!\"out:light" + (i + 1) + "\" | !\"out:light" + (i + 3) + "\") & ";
			}
			safeB = safeB + System.lineSeparator();
		}

		// For system B: forall i, if all lights that in conflict with light i are red,
		// then light i is green

		for (int i = 1; i <= railways; i++) {
			String toAdd = "(" + conflicts(i, railways) + ") -> \"out:light" + i + "\"";
			safeB = safeB + "(" + toAdd + ") & " + System.lineSeparator();
			System.out.println("toAdd= " + toAdd);
		}

		safeA = safeA + System.lineSeparator();
		safeB = safeB + System.lineSeparator();

		// Now, check which traffic light is frequent
		for (int i = 1; i <= railways; i++) {
			// if counter reached k, don't change its value
			safeA = safeA + "( (" + num2asrt(delay, varnum, i, "in") + ") -> (" + num2Xasrt(delay, varnum, i, "in")
					+ ") ) &" + System.lineSeparator();
			safeB = safeB + "( (" + num2asrt(delay, varnum, i, "out") + ") -> (" + num2Xasrt(delay, varnum, i, "out")
					+ ") ) &" + System.lineSeparator();
			// if counter<k and next the light is green,zero the counter
			safeA = safeA + "( (!(" + num2asrt(delay, varnum, i, "in") + ") & X \"in:light" + i + "\") -> ("
					+ num2Xasrt(0, varnum, i, "in") + ") ) &" + System.lineSeparator();
			safeB = safeB + "( (!(" + num2asrt(delay, varnum, i, "out") + ") & X \"out:light" + i + "\") -> ("
					+ num2Xasrt(0, varnum, i, "out") + ") ) &" + System.lineSeparator();
			// if counter = j<k and next light is red, inc the counter
			for (int j = 0; j < delay; j++) {
				safeA = safeA + "( ((" + num2asrt(j, varnum, i, "in") + ") & !(X \"in:light" + i + "\")) -> ("
						+ num2Xasrt(j + 1, varnum, i, "in") + ") ) &" + System.lineSeparator();
				safeB = safeB + "( ((" + num2asrt(j, varnum, i, "out") + ") & !(X \"out:light" + i + "\")) -> ("
						+ num2Xasrt(j + 1, varnum, i, "out") + ") ) &" + System.lineSeparator();
			}
			safeA = safeA + System.lineSeparator();
			safeB = safeB + System.lineSeparator();
		}

		safeA.trim();
		safeA = safeA.replaceAll("\\s+$", "");
		safeA = safeA.substring(0, safeA.length() - 1);
		safeB.trim();
		safeB = safeB.replaceAll("\\s+$", "");
		safeB = safeB.substring(0, safeB.length() - 1);

		System.out.println("system A safety constrints:" + System.lineSeparator() + safeA + System.lineSeparator()
				+ System.lineSeparator() + System.lineSeparator());
		System.out.println("system B safety constrints:" + System.lineSeparator() + safeB + System.lineSeparator()
				+ System.lineSeparator() + System.lineSeparator());

		// create "justice" constraints
		// first, create "regular" assume-guarantee
		String grk = "";
		for (int i = 1; i <= railways; i++) {
			grk = grk + "((GF(\"in:light" + i + "\")) ->  (GF(\"out:light" + i + "\"))) &" + System.lineSeparator();
		}
		// second, create "frequent" assume-guarantee
		for (int i = 1; i <= railways; i++) {
			grk = grk + "((GF(" + "!(" + num2asrt(delay, varnum, i, "in") + "))) ->  (GF(" + "!("
					+ num2asrt(delay, varnum, i, "out") + ")))) &" + System.lineSeparator();
		}

		grk.trim();
		grk = grk.replaceAll("\\s+$", "");
		grk = grk.substring(0, grk.length() - 1);

		System.out.println("GR(k) formula:" + System.lineSeparator() + grk + System.lineSeparator()
				+ System.lineSeparator() + System.lineSeparator());

		String spec = "";
		spec = initA + System.lineSeparator() + System.lineSeparator() + ";" + System.lineSeparator()
				+ System.lineSeparator() + initB + System.lineSeparator() + System.lineSeparator() + ";"
				+ System.lineSeparator() + System.lineSeparator() + safeA + System.lineSeparator()
				+ System.lineSeparator() + ";" + System.lineSeparator() + System.lineSeparator() + safeB
				+ System.lineSeparator() + System.lineSeparator() + ";" + System.lineSeparator()
				+ System.lineSeparator() + grk;

		System.out.println("the spec is:" + System.lineSeparator() + spec);

		// writing to file
		Files.write(Paths.get(filename), Collections.singletonList(spec), UTF_8, APPEND, CREATE);

	}

	private static String conflicts(int rail, int railways) {

		ArrayList incConflict = new ArrayList();
		if (rail - 3 >= 1 && rail % 2 == 0)
			incConflict.add(rail - 3);
		if (rail - 2 >= 1)
			incConflict.add(rail - 2);
		if (rail - 1 >= 1)
			incConflict.add(rail - 1);
		if (rail + 1 <= railways)
			incConflict.add(rail + 1);
		if (rail + 2 <= railways)
			incConflict.add(rail + 2);
		if (rail + 3 <= railways && rail % 2 == 1)
			incConflict.add(rail + 3);

		String ans = "";
		for (int i = 0; i < incConflict.size(); i++) {
			ans = ans + " !\"out:light" + incConflict.get(i) + "\" &";
		}
		ans = ans.substring(0, ans.length() - 1);
		return ans;
	}

	static String num2asrt(int target, int varnum, int light, String player) {
		int[] vars = new int[varnum];
		for (int i = vars.length - 1; i >= 0; i--) {
			int cand = (int) Math.pow(2, i);
			if (cand <= target) {
				vars[i] = 1;
				target = target - cand;
			}
		}
		String ans = "";
		for (int i = 0; i < vars.length; i++) {
			if (vars[i] == 0)
				ans = ans + " !";
			else
				ans = ans + " ";
			ans = ans + "\"" + player + ":freq" + light + "_" + i + "\" &";
		}
		int lngt = ans.length();
		return ans.substring(1, lngt - 2);
	}

	static String num2Xasrt(int target, int varnum, int light, String player) {
		int[] vars = new int[varnum];
		for (int i = vars.length - 1; i >= 0; i--) {
			int cand = (int) Math.pow(2, i);
			if (cand <= target) {
				vars[i] = 1;
				target = target - cand;
			}
		}
		String ans = "";
		for (int i = 0; i < vars.length; i++) {
			if (vars[i] == 0)
				ans = ans + " !";
			else
				ans = ans + " ";
			ans = ans + "(X \"" + player + ":freq" + light + "_" + i + "\") &";
		}
		int lngt = ans.length();
		return ans.substring(1, lngt - 2);
	}

	static void oldInitCreation(int railways, String initA, String initB, int varnum) {
		for (int i = 1; i <= railways; i++) {
			initA = initA + "!\"in:light" + i + "\" & ";
			initB = initB + "!\"out:light" + i + "\" & ";
		}
		initA = initA + System.lineSeparator();
		initB = initB + System.lineSeparator();

		for (int i = 1; i <= railways; i++) {
			initA = initA + num2asrt(1, varnum, i, "in");
			if (i < railways)
				initA = initA + " &" + System.lineSeparator();
			initB = initB + num2asrt(1, varnum, i, "out");
			if (i < railways)
				initB = initB + " &" + System.lineSeparator();
		}
	}

}
