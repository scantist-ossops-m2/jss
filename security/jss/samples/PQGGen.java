/* 
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is the Netscape Security Services for Java.
 * 
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are 
 * Copyright (C) 1998-2000 Netscape Communications Corporation.  All
 * Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License Version 2 or later (the
 * "GPL"), in which case the provisions of the GPL are applicable 
 * instead of those above.  If you wish to allow use of your 
 * version of this file only under the terms of the GPL and not to
 * allow others to use your version of this file under the MPL,
 * indicate your decision by deleting the provisions above and
 * replace them with the notice and other provisions required by
 * the GPL.  If you do not delete the provisions above, a recipient
 * may use your version of this file under either the MPL or the
 * GPL.
 */


import org.mozilla.jss.*;
import org.mozilla.jss.crypto.*;

/**
 * A command-line utility for generating PQG parameters for DSA
 * Key operations. Can be used for testing.
 * Takes the keysize as the sole argument.
 */
public class PQGGen {
    public static void main(String args[]) {
        int size;
        PQGParams pqg;

      try {

        if(args.length != 1) {
            throw new Exception("Usage: java PQGGen <keysize>");
        }

        size = Integer.parseInt(args[0]);

        System.out.println("Generating PQG parameters for "+size+
                            "-bit keypairs. This could take hours...");

        CryptoManager.initialize();

        pqg = PQGParams.generate(size);

        System.out.println("Generated PQG Parameters.");
        System.out.println("Verifying PQG Parameters. "+
                            "This could take a few minutes...");
        if( ! pqg.paramsAreValid() ) {
            throw new Exception("ERROR: Generated parameters are invalid.");
        }

        System.out.println("Parameters are valid!");
        System.out.println("P: "+pqg.getP());
        System.out.println("Q: "+pqg.getQ());
        System.out.println("G: "+pqg.getG());
        System.out.println("H: "+pqg.getH());
        System.out.println("seed: "+pqg.getSeed());
        System.out.println("counter: "+pqg.getCounter());

      } catch(NumberFormatException e) {
          System.err.println("Invalid key size: "+e);
      } catch(PQGParamGenException e) {
          System.err.println(e);
      } catch(java.security.InvalidParameterException e) {
          System.err.println("Invalid key size: "+e);
      } catch(Exception e) {
          System.err.println(e);
      }
    }
}
