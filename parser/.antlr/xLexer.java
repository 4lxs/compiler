// Generated from /home/svl/Projects/llvm/parser/x.g4 by ANTLR 4.13.1
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue", "this-escape"})
public class xLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.13.1", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		IF=1, ELSE=2, FN=3, LET=4, MODULE=5, TYPE=6, PATH=7, PLUS=8, MINUS=9, 
		STAR=10, SLASH=11, EQEQ=12, NEQ=13, LESS=14, GREATER=15, LEQ=16, GEQ=17, 
		LBRACE=18, RBRACE=19, LPAREN=20, RPAREN=21, SEMICOLON=22, COLON=23, COMMA=24, 
		TO=25, EQ=26, NUMBER=27, IDENT=28, WHITESPACE=29, ANY=30;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"IF", "ELSE", "FN", "LET", "MODULE", "TYPE", "PATH", "PLUS", "MINUS", 
			"STAR", "SLASH", "EQEQ", "NEQ", "LESS", "GREATER", "LEQ", "GEQ", "LBRACE", 
			"RBRACE", "LPAREN", "RPAREN", "SEMICOLON", "COLON", "COMMA", "TO", "EQ", 
			"NUMBER", "IDENT", "WHITESPACE", "ANY"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'if'", "'else'", "'fn'", "'let'", "'module'", "'type'", null, 
			"'+'", "'-'", "'*'", "'/'", "'=='", "'!='", "'<'", "'>'", "'<='", "'>='", 
			"'{'", "'}'", "'('", "')'", "';'", "':'", "','", "'->'", "'='"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "IF", "ELSE", "FN", "LET", "MODULE", "TYPE", "PATH", "PLUS", "MINUS", 
			"STAR", "SLASH", "EQEQ", "NEQ", "LESS", "GREATER", "LEQ", "GEQ", "LBRACE", 
			"RBRACE", "LPAREN", "RPAREN", "SEMICOLON", "COLON", "COMMA", "TO", "EQ", 
			"NUMBER", "IDENT", "WHITESPACE", "ANY"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}


	public xLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "x.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\u0004\u0000\u001e\u00a7\u0006\uffff\uffff\u0002\u0000\u0007\u0000\u0002"+
		"\u0001\u0007\u0001\u0002\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0002"+
		"\u0004\u0007\u0004\u0002\u0005\u0007\u0005\u0002\u0006\u0007\u0006\u0002"+
		"\u0007\u0007\u0007\u0002\b\u0007\b\u0002\t\u0007\t\u0002\n\u0007\n\u0002"+
		"\u000b\u0007\u000b\u0002\f\u0007\f\u0002\r\u0007\r\u0002\u000e\u0007\u000e"+
		"\u0002\u000f\u0007\u000f\u0002\u0010\u0007\u0010\u0002\u0011\u0007\u0011"+
		"\u0002\u0012\u0007\u0012\u0002\u0013\u0007\u0013\u0002\u0014\u0007\u0014"+
		"\u0002\u0015\u0007\u0015\u0002\u0016\u0007\u0016\u0002\u0017\u0007\u0017"+
		"\u0002\u0018\u0007\u0018\u0002\u0019\u0007\u0019\u0002\u001a\u0007\u001a"+
		"\u0002\u001b\u0007\u001b\u0002\u001c\u0007\u001c\u0002\u001d\u0007\u001d"+
		"\u0001\u0000\u0001\u0000\u0001\u0000\u0001\u0001\u0001\u0001\u0001\u0001"+
		"\u0001\u0001\u0001\u0001\u0001\u0002\u0001\u0002\u0001\u0002\u0001\u0003"+
		"\u0001\u0003\u0001\u0003\u0001\u0003\u0001\u0004\u0001\u0004\u0001\u0004"+
		"\u0001\u0004\u0001\u0004\u0001\u0004\u0001\u0004\u0001\u0005\u0001\u0005"+
		"\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0006\u0005\u0006Z\b\u0006"+
		"\n\u0006\f\u0006]\t\u0006\u0001\u0006\u0001\u0006\u0001\u0006\u0001\u0006"+
		"\u0004\u0006c\b\u0006\u000b\u0006\f\u0006d\u0004\u0006g\b\u0006\u000b"+
		"\u0006\f\u0006h\u0001\u0007\u0001\u0007\u0001\b\u0001\b\u0001\t\u0001"+
		"\t\u0001\n\u0001\n\u0001\u000b\u0001\u000b\u0001\u000b\u0001\f\u0001\f"+
		"\u0001\f\u0001\r\u0001\r\u0001\u000e\u0001\u000e\u0001\u000f\u0001\u000f"+
		"\u0001\u000f\u0001\u0010\u0001\u0010\u0001\u0010\u0001\u0011\u0001\u0011"+
		"\u0001\u0012\u0001\u0012\u0001\u0013\u0001\u0013\u0001\u0014\u0001\u0014"+
		"\u0001\u0015\u0001\u0015\u0001\u0016\u0001\u0016\u0001\u0017\u0001\u0017"+
		"\u0001\u0018\u0001\u0018\u0001\u0018\u0001\u0019\u0001\u0019\u0001\u001a"+
		"\u0004\u001a\u0097\b\u001a\u000b\u001a\f\u001a\u0098\u0001\u001b\u0001"+
		"\u001b\u0005\u001b\u009d\b\u001b\n\u001b\f\u001b\u00a0\t\u001b\u0001\u001c"+
		"\u0001\u001c\u0001\u001c\u0001\u001c\u0001\u001d\u0001\u001d\u0000\u0000"+
		"\u001e\u0001\u0001\u0003\u0002\u0005\u0003\u0007\u0004\t\u0005\u000b\u0006"+
		"\r\u0007\u000f\b\u0011\t\u0013\n\u0015\u000b\u0017\f\u0019\r\u001b\u000e"+
		"\u001d\u000f\u001f\u0010!\u0011#\u0012%\u0013\'\u0014)\u0015+\u0016-\u0017"+
		"/\u00181\u00193\u001a5\u001b7\u001c9\u001d;\u001e\u0001\u0000\u0005\u0001"+
		"\u0000az\u0001\u000009\u0003\u0000AZ__az\u0004\u000009AZ__az\u0002\u0000"+
		"\n\n  \u00ab\u0000\u0001\u0001\u0000\u0000\u0000\u0000\u0003\u0001\u0000"+
		"\u0000\u0000\u0000\u0005\u0001\u0000\u0000\u0000\u0000\u0007\u0001\u0000"+
		"\u0000\u0000\u0000\t\u0001\u0000\u0000\u0000\u0000\u000b\u0001\u0000\u0000"+
		"\u0000\u0000\r\u0001\u0000\u0000\u0000\u0000\u000f\u0001\u0000\u0000\u0000"+
		"\u0000\u0011\u0001\u0000\u0000\u0000\u0000\u0013\u0001\u0000\u0000\u0000"+
		"\u0000\u0015\u0001\u0000\u0000\u0000\u0000\u0017\u0001\u0000\u0000\u0000"+
		"\u0000\u0019\u0001\u0000\u0000\u0000\u0000\u001b\u0001\u0000\u0000\u0000"+
		"\u0000\u001d\u0001\u0000\u0000\u0000\u0000\u001f\u0001\u0000\u0000\u0000"+
		"\u0000!\u0001\u0000\u0000\u0000\u0000#\u0001\u0000\u0000\u0000\u0000%"+
		"\u0001\u0000\u0000\u0000\u0000\'\u0001\u0000\u0000\u0000\u0000)\u0001"+
		"\u0000\u0000\u0000\u0000+\u0001\u0000\u0000\u0000\u0000-\u0001\u0000\u0000"+
		"\u0000\u0000/\u0001\u0000\u0000\u0000\u00001\u0001\u0000\u0000\u0000\u0000"+
		"3\u0001\u0000\u0000\u0000\u00005\u0001\u0000\u0000\u0000\u00007\u0001"+
		"\u0000\u0000\u0000\u00009\u0001\u0000\u0000\u0000\u0000;\u0001\u0000\u0000"+
		"\u0000\u0001=\u0001\u0000\u0000\u0000\u0003@\u0001\u0000\u0000\u0000\u0005"+
		"E\u0001\u0000\u0000\u0000\u0007H\u0001\u0000\u0000\u0000\tL\u0001\u0000"+
		"\u0000\u0000\u000bS\u0001\u0000\u0000\u0000\r[\u0001\u0000\u0000\u0000"+
		"\u000fj\u0001\u0000\u0000\u0000\u0011l\u0001\u0000\u0000\u0000\u0013n"+
		"\u0001\u0000\u0000\u0000\u0015p\u0001\u0000\u0000\u0000\u0017r\u0001\u0000"+
		"\u0000\u0000\u0019u\u0001\u0000\u0000\u0000\u001bx\u0001\u0000\u0000\u0000"+
		"\u001dz\u0001\u0000\u0000\u0000\u001f|\u0001\u0000\u0000\u0000!\u007f"+
		"\u0001\u0000\u0000\u0000#\u0082\u0001\u0000\u0000\u0000%\u0084\u0001\u0000"+
		"\u0000\u0000\'\u0086\u0001\u0000\u0000\u0000)\u0088\u0001\u0000\u0000"+
		"\u0000+\u008a\u0001\u0000\u0000\u0000-\u008c\u0001\u0000\u0000\u0000/"+
		"\u008e\u0001\u0000\u0000\u00001\u0090\u0001\u0000\u0000\u00003\u0093\u0001"+
		"\u0000\u0000\u00005\u0096\u0001\u0000\u0000\u00007\u009a\u0001\u0000\u0000"+
		"\u00009\u00a1\u0001\u0000\u0000\u0000;\u00a5\u0001\u0000\u0000\u0000="+
		">\u0005i\u0000\u0000>?\u0005f\u0000\u0000?\u0002\u0001\u0000\u0000\u0000"+
		"@A\u0005e\u0000\u0000AB\u0005l\u0000\u0000BC\u0005s\u0000\u0000CD\u0005"+
		"e\u0000\u0000D\u0004\u0001\u0000\u0000\u0000EF\u0005f\u0000\u0000FG\u0005"+
		"n\u0000\u0000G\u0006\u0001\u0000\u0000\u0000HI\u0005l\u0000\u0000IJ\u0005"+
		"e\u0000\u0000JK\u0005t\u0000\u0000K\b\u0001\u0000\u0000\u0000LM\u0005"+
		"m\u0000\u0000MN\u0005o\u0000\u0000NO\u0005d\u0000\u0000OP\u0005u\u0000"+
		"\u0000PQ\u0005l\u0000\u0000QR\u0005e\u0000\u0000R\n\u0001\u0000\u0000"+
		"\u0000ST\u0005t\u0000\u0000TU\u0005y\u0000\u0000UV\u0005p\u0000\u0000"+
		"VW\u0005e\u0000\u0000W\f\u0001\u0000\u0000\u0000XZ\u0007\u0000\u0000\u0000"+
		"YX\u0001\u0000\u0000\u0000Z]\u0001\u0000\u0000\u0000[Y\u0001\u0000\u0000"+
		"\u0000[\\\u0001\u0000\u0000\u0000\\f\u0001\u0000\u0000\u0000][\u0001\u0000"+
		"\u0000\u0000^_\u0005:\u0000\u0000_`\u0005:\u0000\u0000`b\u0001\u0000\u0000"+
		"\u0000ac\u0007\u0000\u0000\u0000ba\u0001\u0000\u0000\u0000cd\u0001\u0000"+
		"\u0000\u0000db\u0001\u0000\u0000\u0000de\u0001\u0000\u0000\u0000eg\u0001"+
		"\u0000\u0000\u0000f^\u0001\u0000\u0000\u0000gh\u0001\u0000\u0000\u0000"+
		"hf\u0001\u0000\u0000\u0000hi\u0001\u0000\u0000\u0000i\u000e\u0001\u0000"+
		"\u0000\u0000jk\u0005+\u0000\u0000k\u0010\u0001\u0000\u0000\u0000lm\u0005"+
		"-\u0000\u0000m\u0012\u0001\u0000\u0000\u0000no\u0005*\u0000\u0000o\u0014"+
		"\u0001\u0000\u0000\u0000pq\u0005/\u0000\u0000q\u0016\u0001\u0000\u0000"+
		"\u0000rs\u0005=\u0000\u0000st\u0005=\u0000\u0000t\u0018\u0001\u0000\u0000"+
		"\u0000uv\u0005!\u0000\u0000vw\u0005=\u0000\u0000w\u001a\u0001\u0000\u0000"+
		"\u0000xy\u0005<\u0000\u0000y\u001c\u0001\u0000\u0000\u0000z{\u0005>\u0000"+
		"\u0000{\u001e\u0001\u0000\u0000\u0000|}\u0005<\u0000\u0000}~\u0005=\u0000"+
		"\u0000~ \u0001\u0000\u0000\u0000\u007f\u0080\u0005>\u0000\u0000\u0080"+
		"\u0081\u0005=\u0000\u0000\u0081\"\u0001\u0000\u0000\u0000\u0082\u0083"+
		"\u0005{\u0000\u0000\u0083$\u0001\u0000\u0000\u0000\u0084\u0085\u0005}"+
		"\u0000\u0000\u0085&\u0001\u0000\u0000\u0000\u0086\u0087\u0005(\u0000\u0000"+
		"\u0087(\u0001\u0000\u0000\u0000\u0088\u0089\u0005)\u0000\u0000\u0089*"+
		"\u0001\u0000\u0000\u0000\u008a\u008b\u0005;\u0000\u0000\u008b,\u0001\u0000"+
		"\u0000\u0000\u008c\u008d\u0005:\u0000\u0000\u008d.\u0001\u0000\u0000\u0000"+
		"\u008e\u008f\u0005,\u0000\u0000\u008f0\u0001\u0000\u0000\u0000\u0090\u0091"+
		"\u0005-\u0000\u0000\u0091\u0092\u0005>\u0000\u0000\u00922\u0001\u0000"+
		"\u0000\u0000\u0093\u0094\u0005=\u0000\u0000\u00944\u0001\u0000\u0000\u0000"+
		"\u0095\u0097\u0007\u0001\u0000\u0000\u0096\u0095\u0001\u0000\u0000\u0000"+
		"\u0097\u0098\u0001\u0000\u0000\u0000\u0098\u0096\u0001\u0000\u0000\u0000"+
		"\u0098\u0099\u0001\u0000\u0000\u0000\u00996\u0001\u0000\u0000\u0000\u009a"+
		"\u009e\u0007\u0002\u0000\u0000\u009b\u009d\u0007\u0003\u0000\u0000\u009c"+
		"\u009b\u0001\u0000\u0000\u0000\u009d\u00a0\u0001\u0000\u0000\u0000\u009e"+
		"\u009c\u0001\u0000\u0000\u0000\u009e\u009f\u0001\u0000\u0000\u0000\u009f"+
		"8\u0001\u0000\u0000\u0000\u00a0\u009e\u0001\u0000\u0000\u0000\u00a1\u00a2"+
		"\u0007\u0004\u0000\u0000\u00a2\u00a3\u0001\u0000\u0000\u0000\u00a3\u00a4"+
		"\u0006\u001c\u0000\u0000\u00a4:\u0001\u0000\u0000\u0000\u00a5\u00a6\t"+
		"\u0000\u0000\u0000\u00a6<\u0001\u0000\u0000\u0000\u0006\u0000[dh\u0098"+
		"\u009e\u0001\u0006\u0000\u0000";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}