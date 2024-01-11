import java.io.*;
import java.util.*;


public class Markov {


    class Prefix {
        public Vector pref;
        public String suffix;

        Prefix(int prefix, String word)
        {
            System.out.println("Prefix ctor");
        }

    }

    class Chain {
        static final int NPREF = 2;
        static final String NONWORD = "\n";
        Hashtable statetab = new Hashtable();
        Prefix prefix = new Prefix(NPREF, NONWORD);
        Random rand = new Random();

        public void build(InputStream in) //throws IOException
        {
            System.out.println("Chain: build");
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
        Chain chain = new Chain();
        int nwords = MAXGEN;
        System.out.println("dude, wudup");
        chain.build(System.in);
        chain.generate(nwords);
    }
}
