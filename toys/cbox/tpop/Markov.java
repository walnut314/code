import java.io.*;
import java.util.*;

public class Markov {

    class Prefix {
        public Vector pref;
        public String suffix;

        Prefix(int prefix, String word)
        {
            System.out.println("Prefix1 ctor");
        }

        Prefix(Prefix prefix)
        {
            System.out.println("Prefix2 ctor");
        }
    }

    class Chain {
        static final int NPREF = 2;
        static final String NONWORD = "\n";
        Hashtable statetab = new Hashtable();
        Prefix prefix = new Prefix(NPREF, NONWORD);
        Random rand = new Random();

        public void build(InputStream in) throws IOException
        {
            System.out.println("Chain: build");
            StreamTokenizer st = new StreamTokenizer(in);
            st.resetSyntax();
            st.wordChars(0, Character.MAX_VALUE);
            st.whitespaceChars(0, ' ');
            while (st.nextToken() != st.TT_EOF) {
                add(st.sval);
            }
        }

        private void add(String word)
        {
            Vector suf = (Vector) statetab.get(prefix);
            if (suf == null) {
                suf = new Vector();
                statetab.put(new Prefix(prefix), suf);
            }
            suf.addElement(word);
            prefix.pref.removeElementAt(0);
            prefix.pref.addElement(word);
        }

        public void generate(int nwords)
        {
            System.out.println("Chain: generate");
        }

    }

    static final int MAXGEN = 10000;
    public static void main(String[] args)
    {
        Markov markov = new Markov();
        markov.run();
    }

    public void run()
    {
        int nwords = MAXGEN;
        Chain chain = new Chain();
        System.out.println("dude, wudup");

        try {
            File file = new File("markov-chain-test-text.txt");
            FileInputStream fileInputStream = new FileInputStream(file);
            chain.build(fileInputStream);
        } catch (IOException ioexcpt)
        {
            System.out.println("bang");
        } 
        chain.generate(nwords);
    }
}
