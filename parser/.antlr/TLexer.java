// Generated from /home/svl/Projects/llvm/parser/TLexer.g4 by ANTLR 4.13.1
/* lexer header section */
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue", "this-escape"})
public class TLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.13.1", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		DUMMY=1, Return=2, Continue=3, INT=4, Digit=5, ID=6, LessThan=7, GreaterThan=8, 
		Equal=9, And=10, Colon=11, Semicolon=12, Plus=13, Minus=14, Star=15, OpenPar=16, 
		ClosePar=17, OpenCurly=18, CloseCurly=19, QuestionMark=20, Comma=21, String=22, 
		Foo=23, Bar=24, Any=25, Comment=26, WS=27, Dot=28, DotDot=29, Dollar=30, 
		Ampersand=31;
	public static final int
		CommentsChannel=2, DirectiveChannel=3;
	public static final int
		Mode1=1, Mode2=2;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN", "CommentsChannel", "DirectiveChannel"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE", "Mode1", "Mode2"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"Return", "Continue", "INT", "Digit", "ID", "LETTER", "LessThan", "GreaterThan", 
			"Equal", "And", "Colon", "Semicolon", "Plus", "Minus", "Star", "OpenPar", 
			"ClosePar", "OpenCurly", "CloseCurly", "QuestionMark", "Comma", "Dollar", 
			"Ampersand", "String", "Foo", "Bar", "Any", "Comment", "WS", "Baz", "Dot", 
			"DotDot"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, null, "'return'", "'continue'", null, null, null, "'<'", "'>'", 
			"'='", "'and'", "':'", "';'", "'+'", "'-'", "'*'", "'('", "')'", "'{'", 
			"'}'", "'?'", "','", null, null, null, null, null, null, "'.'", "'..'", 
			"'$'", "'&'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "DUMMY", "Return", "Continue", "INT", "Digit", "ID", "LessThan", 
			"GreaterThan", "Equal", "And", "Colon", "Semicolon", "Plus", "Minus", 
			"Star", "OpenPar", "ClosePar", "OpenCurly", "CloseCurly", "QuestionMark", 
			"Comma", "String", "Foo", "Bar", "Any", "Comment", "WS", "Dot", "DotDot", 
			"Dollar", "Ampersand"
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

	/* public lexer declarations section */
	bool canTestFoo() { return true; }
	bool isItFoo() { return true; }
	bool isItBar() { return true; }

	void myFooLexerAction() { /* do something*/ };
	void myBarLexerAction() { /* do something*/ };


	public TLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "TLexer.g4"; }

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

	@Override
	public void action(RuleContext _localctx, int ruleIndex, int actionIndex) {
		switch (ruleIndex) {
		case 24:
			Foo_action((RuleContext)_localctx, actionIndex);
			break;
		case 25:
			Bar_action((RuleContext)_localctx, actionIndex);
			break;
		}
	}
	private void Foo_action(RuleContext _localctx, int actionIndex) {
		switch (actionIndex) {
		case 0:
			 myFooLexerAction(); 
			break;
		}
	}
	private void Bar_action(RuleContext _localctx, int actionIndex) {
		switch (actionIndex) {
		case 1:
			 myBarLexerAction(); 
			break;
		}
	}
	@Override
	public boolean sempred(RuleContext _localctx, int ruleIndex, int predIndex) {
		switch (ruleIndex) {
		case 24:
			return Foo_sempred((RuleContext)_localctx, predIndex);
		case 25:
			return Bar_sempred((RuleContext)_localctx, predIndex);
		}
		return true;
	}
	private boolean Foo_sempred(RuleContext _localctx, int predIndex) {
		switch (predIndex) {
		case 0:
			return canTestFoo();
		case 1:
			return isItFoo();
		}
		return true;
	}
	private boolean Bar_sempred(RuleContext _localctx, int predIndex) {
		switch (predIndex) {
		case 2:
			return isItBar();
		}
		return true;
	}

	public static final String _serializedATN =
		"\u0004\u0000\u001f\u00d1\u0006\uffff\uffff\u0006\uffff\uffff\u0006\uffff"+
		"\uffff\u0002\u0000\u0007\u0000\u0002\u0001\u0007\u0001\u0002\u0002\u0007"+
		"\u0002\u0002\u0003\u0007\u0003\u0002\u0004\u0007\u0004\u0002\u0005\u0007"+
		"\u0005\u0002\u0006\u0007\u0006\u0002\u0007\u0007\u0007\u0002\b\u0007\b"+
		"\u0002\t\u0007\t\u0002\n\u0007\n\u0002\u000b\u0007\u000b\u0002\f\u0007"+
		"\f\u0002\r\u0007\r\u0002\u000e\u0007\u000e\u0002\u000f\u0007\u000f\u0002"+
		"\u0010\u0007\u0010\u0002\u0011\u0007\u0011\u0002\u0012\u0007\u0012\u0002"+
		"\u0013\u0007\u0013\u0002\u0014\u0007\u0014\u0002\u0015\u0007\u0015\u0002"+
		"\u0016\u0007\u0016\u0002\u0017\u0007\u0017\u0002\u0018\u0007\u0018\u0002"+
		"\u0019\u0007\u0019\u0002\u001a\u0007\u001a\u0002\u001b\u0007\u001b\u0002"+
		"\u001c\u0007\u001c\u0002\u001d\u0007\u001d\u0002\u001e\u0007\u001e\u0002"+
		"\u001f\u0007\u001f\u0001\u0000\u0001\u0000\u0001\u0000\u0001\u0000\u0001"+
		"\u0000\u0001\u0000\u0001\u0000\u0001\u0001\u0001\u0001\u0001\u0001\u0001"+
		"\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001"+
		"\u0002\u0004\u0002U\b\u0002\u000b\u0002\f\u0002V\u0001\u0003\u0001\u0003"+
		"\u0001\u0004\u0001\u0004\u0001\u0004\u0005\u0004^\b\u0004\n\u0004\f\u0004"+
		"a\t\u0004\u0001\u0005\u0001\u0005\u0001\u0006\u0001\u0006\u0001\u0007"+
		"\u0001\u0007\u0001\b\u0001\b\u0001\t\u0001\t\u0001\t\u0001\t\u0001\n\u0001"+
		"\n\u0001\u000b\u0001\u000b\u0001\f\u0001\f\u0001\r\u0001\r\u0001\u000e"+
		"\u0001\u000e\u0001\u000f\u0001\u000f\u0001\u0010\u0001\u0010\u0001\u0011"+
		"\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0012\u0001\u0012\u0001\u0012"+
		"\u0001\u0012\u0001\u0013\u0001\u0013\u0001\u0014\u0001\u0014\u0001\u0014"+
		"\u0001\u0014\u0001\u0015\u0001\u0015\u0001\u0015\u0001\u0015\u0001\u0015"+
		"\u0001\u0016\u0001\u0016\u0001\u0016\u0001\u0016\u0001\u0017\u0001\u0017"+
		"\u0005\u0017\u0096\b\u0017\n\u0017\f\u0017\u0099\t\u0017\u0001\u0017\u0001"+
		"\u0017\u0001\u0018\u0001\u0018\u0001\u0018\u0001\u0018\u0001\u0018\u0001"+
		"\u0018\u0001\u0018\u0001\u0018\u0001\u0019\u0001\u0019\u0001\u0019\u0001"+
		"\u0019\u0001\u0019\u0001\u0019\u0001\u0019\u0001\u001a\u0001\u001a\u0001"+
		"\u001a\u0003\u001a\u00af\b\u001a\u0001\u001a\u0001\u001a\u0001\u001a\u0001"+
		"\u001b\u0001\u001b\u0005\u001b\u00b6\b\u001b\n\u001b\f\u001b\u00b9\t\u001b"+
		"\u0001\u001b\u0003\u001b\u00bc\b\u001b\u0001\u001b\u0001\u001b\u0001\u001b"+
		"\u0001\u001b\u0001\u001c\u0004\u001c\u00c3\b\u001c\u000b\u001c\f\u001c"+
		"\u00c4\u0001\u001c\u0001\u001c\u0001\u001d\u0001\u001d\u0001\u001d\u0001"+
		"\u001d\u0001\u001e\u0001\u001e\u0001\u001f\u0001\u001f\u0001\u001f\u0001"+
		"\u0097\u0000 \u0003\u0002\u0005\u0003\u0007\u0004\t\u0005\u000b\u0006"+
		"\r\u0000\u000f\u0007\u0011\b\u0013\t\u0015\n\u0017\u000b\u0019\f\u001b"+
		"\r\u001d\u000e\u001f\u000f!\u0010#\u0011%\u0012\'\u0013)\u0014+\u0015"+
		"-\u001e/\u001f1\u00163\u00175\u00187\u00199\u001a;\u001b=\u0000?\u001c"+
		"A\u001d\u0003\u0000\u0001\u0002\u0004\u0001\u000009\u0003\u0000AZaz\u0080"+
		"\u8010\uffff\u0002\u0000\n\n\r\r\u0003\u0000\t\n\r\r  \u00d4\u0000\u0003"+
		"\u0001\u0000\u0000\u0000\u0000\u0005\u0001\u0000\u0000\u0000\u0000\u0007"+
		"\u0001\u0000\u0000\u0000\u0000\t\u0001\u0000\u0000\u0000\u0000\u000b\u0001"+
		"\u0000\u0000\u0000\u0000\u000f\u0001\u0000\u0000\u0000\u0000\u0011\u0001"+
		"\u0000\u0000\u0000\u0000\u0013\u0001\u0000\u0000\u0000\u0000\u0015\u0001"+
		"\u0000\u0000\u0000\u0000\u0017\u0001\u0000\u0000\u0000\u0000\u0019\u0001"+
		"\u0000\u0000\u0000\u0000\u001b\u0001\u0000\u0000\u0000\u0000\u001d\u0001"+
		"\u0000\u0000\u0000\u0000\u001f\u0001\u0000\u0000\u0000\u0000!\u0001\u0000"+
		"\u0000\u0000\u0000#\u0001\u0000\u0000\u0000\u0000%\u0001\u0000\u0000\u0000"+
		"\u0000\'\u0001\u0000\u0000\u0000\u0000)\u0001\u0000\u0000\u0000\u0000"+
		"+\u0001\u0000\u0000\u0000\u0000-\u0001\u0000\u0000\u0000\u0000/\u0001"+
		"\u0000\u0000\u0000\u00001\u0001\u0000\u0000\u0000\u00003\u0001\u0000\u0000"+
		"\u0000\u00005\u0001\u0000\u0000\u0000\u00007\u0001\u0000\u0000\u0000\u0000"+
		"9\u0001\u0000\u0000\u0000\u0000;\u0001\u0000\u0000\u0000\u0001?\u0001"+
		"\u0000\u0000\u0000\u0002A\u0001\u0000\u0000\u0000\u0003C\u0001\u0000\u0000"+
		"\u0000\u0005J\u0001\u0000\u0000\u0000\u0007T\u0001\u0000\u0000\u0000\t"+
		"X\u0001\u0000\u0000\u0000\u000bZ\u0001\u0000\u0000\u0000\rb\u0001\u0000"+
		"\u0000\u0000\u000fd\u0001\u0000\u0000\u0000\u0011f\u0001\u0000\u0000\u0000"+
		"\u0013h\u0001\u0000\u0000\u0000\u0015j\u0001\u0000\u0000\u0000\u0017n"+
		"\u0001\u0000\u0000\u0000\u0019p\u0001\u0000\u0000\u0000\u001br\u0001\u0000"+
		"\u0000\u0000\u001dt\u0001\u0000\u0000\u0000\u001fv\u0001\u0000\u0000\u0000"+
		"!x\u0001\u0000\u0000\u0000#z\u0001\u0000\u0000\u0000%|\u0001\u0000\u0000"+
		"\u0000\'\u0080\u0001\u0000\u0000\u0000)\u0084\u0001\u0000\u0000\u0000"+
		"+\u0086\u0001\u0000\u0000\u0000-\u008a\u0001\u0000\u0000\u0000/\u008f"+
		"\u0001\u0000\u0000\u00001\u0093\u0001\u0000\u0000\u00003\u009c\u0001\u0000"+
		"\u0000\u00005\u00a4\u0001\u0000\u0000\u00007\u00ab\u0001\u0000\u0000\u0000"+
		"9\u00b3\u0001\u0000\u0000\u0000;\u00c2\u0001\u0000\u0000\u0000=\u00c8"+
		"\u0001\u0000\u0000\u0000?\u00cc\u0001\u0000\u0000\u0000A\u00ce\u0001\u0000"+
		"\u0000\u0000CD\u0005r\u0000\u0000DE\u0005e\u0000\u0000EF\u0005t\u0000"+
		"\u0000FG\u0005u\u0000\u0000GH\u0005r\u0000\u0000HI\u0005n\u0000\u0000"+
		"I\u0004\u0001\u0000\u0000\u0000JK\u0005c\u0000\u0000KL\u0005o\u0000\u0000"+
		"LM\u0005n\u0000\u0000MN\u0005t\u0000\u0000NO\u0005i\u0000\u0000OP\u0005"+
		"n\u0000\u0000PQ\u0005u\u0000\u0000QR\u0005e\u0000\u0000R\u0006\u0001\u0000"+
		"\u0000\u0000SU\u0003\t\u0003\u0000TS\u0001\u0000\u0000\u0000UV\u0001\u0000"+
		"\u0000\u0000VT\u0001\u0000\u0000\u0000VW\u0001\u0000\u0000\u0000W\b\u0001"+
		"\u0000\u0000\u0000XY\u0007\u0000\u0000\u0000Y\n\u0001\u0000\u0000\u0000"+
		"Z_\u0003\r\u0005\u0000[^\u0003\r\u0005\u0000\\^\u000209\u0000][\u0001"+
		"\u0000\u0000\u0000]\\\u0001\u0000\u0000\u0000^a\u0001\u0000\u0000\u0000"+
		"_]\u0001\u0000\u0000\u0000_`\u0001\u0000\u0000\u0000`\f\u0001\u0000\u0000"+
		"\u0000a_\u0001\u0000\u0000\u0000bc\u0007\u0001\u0000\u0000c\u000e\u0001"+
		"\u0000\u0000\u0000de\u0005<\u0000\u0000e\u0010\u0001\u0000\u0000\u0000"+
		"fg\u0005>\u0000\u0000g\u0012\u0001\u0000\u0000\u0000hi\u0005=\u0000\u0000"+
		"i\u0014\u0001\u0000\u0000\u0000jk\u0005a\u0000\u0000kl\u0005n\u0000\u0000"+
		"lm\u0005d\u0000\u0000m\u0016\u0001\u0000\u0000\u0000no\u0005:\u0000\u0000"+
		"o\u0018\u0001\u0000\u0000\u0000pq\u0005;\u0000\u0000q\u001a\u0001\u0000"+
		"\u0000\u0000rs\u0005+\u0000\u0000s\u001c\u0001\u0000\u0000\u0000tu\u0005"+
		"-\u0000\u0000u\u001e\u0001\u0000\u0000\u0000vw\u0005*\u0000\u0000w \u0001"+
		"\u0000\u0000\u0000xy\u0005(\u0000\u0000y\"\u0001\u0000\u0000\u0000z{\u0005"+
		")\u0000\u0000{$\u0001\u0000\u0000\u0000|}\u0005{\u0000\u0000}~\u0001\u0000"+
		"\u0000\u0000~\u007f\u0006\u0011\u0000\u0000\u007f&\u0001\u0000\u0000\u0000"+
		"\u0080\u0081\u0005}\u0000\u0000\u0081\u0082\u0001\u0000\u0000\u0000\u0082"+
		"\u0083\u0006\u0012\u0001\u0000\u0083(\u0001\u0000\u0000\u0000\u0084\u0085"+
		"\u0005?\u0000\u0000\u0085*\u0001\u0000\u0000\u0000\u0086\u0087\u0005,"+
		"\u0000\u0000\u0087\u0088\u0001\u0000\u0000\u0000\u0088\u0089\u0006\u0014"+
		"\u0002\u0000\u0089,\u0001\u0000\u0000\u0000\u008a\u008b\u0005$\u0000\u0000"+
		"\u008b\u008c\u0001\u0000\u0000\u0000\u008c\u008d\u0006\u0015\u0003\u0000"+
		"\u008d\u008e\u0006\u0015\u0004\u0000\u008e.\u0001\u0000\u0000\u0000\u008f"+
		"\u0090\u0005&\u0000\u0000\u0090\u0091\u0001\u0000\u0000\u0000\u0091\u0092"+
		"\u0006\u0016\u0005\u0000\u00920\u0001\u0000\u0000\u0000\u0093\u0097\u0005"+
		"\"\u0000\u0000\u0094\u0096\t\u0000\u0000\u0000\u0095\u0094\u0001\u0000"+
		"\u0000\u0000\u0096\u0099\u0001\u0000\u0000\u0000\u0097\u0098\u0001\u0000"+
		"\u0000\u0000\u0097\u0095\u0001\u0000\u0000\u0000\u0098\u009a\u0001\u0000"+
		"\u0000\u0000\u0099\u0097\u0001\u0000\u0000\u0000\u009a\u009b\u0005\"\u0000"+
		"\u0000\u009b2\u0001\u0000\u0000\u0000\u009c\u009d\u0004\u0018\u0000\u0000"+
		"\u009d\u009e\u0005f\u0000\u0000\u009e\u009f\u0005o\u0000\u0000\u009f\u00a0"+
		"\u0005o\u0000\u0000\u00a0\u00a1\u0001\u0000\u0000\u0000\u00a1\u00a2\u0004"+
		"\u0018\u0001\u0000\u00a2\u00a3\u0006\u0018\u0006\u0000\u00a34\u0001\u0000"+
		"\u0000\u0000\u00a4\u00a5\u0005b\u0000\u0000\u00a5\u00a6\u0005a\u0000\u0000"+
		"\u00a6\u00a7\u0005r\u0000\u0000\u00a7\u00a8\u0001\u0000\u0000\u0000\u00a8"+
		"\u00a9\u0004\u0019\u0002\u0000\u00a9\u00aa\u0006\u0019\u0007\u0000\u00aa"+
		"6\u0001\u0000\u0000\u0000\u00ab\u00ac\u00033\u0018\u0000\u00ac\u00ae\u0003"+
		"?\u001e\u0000\u00ad\u00af\u00035\u0019\u0000\u00ae\u00ad\u0001\u0000\u0000"+
		"\u0000\u00ae\u00af\u0001\u0000\u0000\u0000\u00af\u00b0\u0001\u0000\u0000"+
		"\u0000\u00b0\u00b1\u0003A\u001f\u0000\u00b1\u00b2\u0003=\u001d\u0000\u00b2"+
		"8\u0001\u0000\u0000\u0000\u00b3\u00b7\u0005#\u0000\u0000\u00b4\u00b6\b"+
		"\u0002\u0000\u0000\u00b5\u00b4\u0001\u0000\u0000\u0000\u00b6\u00b9\u0001"+
		"\u0000\u0000\u0000\u00b7\u00b5\u0001\u0000\u0000\u0000\u00b7\u00b8\u0001"+
		"\u0000\u0000\u0000\u00b8\u00bb\u0001\u0000\u0000\u0000\u00b9\u00b7\u0001"+
		"\u0000\u0000\u0000\u00ba\u00bc\u0005\r\u0000\u0000\u00bb\u00ba\u0001\u0000"+
		"\u0000\u0000\u00bb\u00bc\u0001\u0000\u0000\u0000\u00bc\u00bd\u0001\u0000"+
		"\u0000\u0000\u00bd\u00be\u0005\n\u0000\u0000\u00be\u00bf\u0001\u0000\u0000"+
		"\u0000\u00bf\u00c0\u0006\u001b\b\u0000\u00c0:\u0001\u0000\u0000\u0000"+
		"\u00c1\u00c3\u0007\u0003\u0000\u0000\u00c2\u00c1\u0001\u0000\u0000\u0000"+
		"\u00c3\u00c4\u0001\u0000\u0000\u0000\u00c4\u00c2\u0001\u0000\u0000\u0000"+
		"\u00c4\u00c5\u0001\u0000\u0000\u0000\u00c5\u00c6\u0001\u0000\u0000\u0000"+
		"\u00c6\u00c7\u0006\u001c\t\u0000\u00c7<\u0001\u0000\u0000\u0000\u00c8"+
		"\u00c9\u0005B\u0000\u0000\u00c9\u00ca\u0005a\u0000\u0000\u00ca\u00cb\u0005"+
		"z\u0000\u0000\u00cb>\u0001\u0000\u0000\u0000\u00cc\u00cd\u0005.\u0000"+
		"\u0000\u00cd@\u0001\u0000\u0000\u0000\u00ce\u00cf\u0005.\u0000\u0000\u00cf"+
		"\u00d0\u0005.\u0000\u0000\u00d0B\u0001\u0000\u0000\u0000\u000b\u0000\u0001"+
		"\u0002V]_\u0097\u00ae\u00b7\u00bb\u00c4\n\u0005\u0001\u0000\u0004\u0000"+
		"\u0000\u0006\u0000\u0000\u0003\u0000\u0000\u0002\u0001\u0000\u0007\u0001"+
		"\u0000\u0001\u0018\u0000\u0001\u0019\u0001\u0000\u0002\u0000\u0000c\u0000";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}