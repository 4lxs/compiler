// Generated from /home/svl/Projects/llvm/parser/TParser.g4 by ANTLR 4.13.1
/* parser/listener/visitor header section */
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue"})
public class TParser extends Parser {
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
		RULE_main = 0, RULE_divide = 1, RULE_and_ = 2, RULE_conquer = 3, RULE_unused = 4, 
		RULE_unused2 = 5, RULE_stat = 6, RULE_expr = 7, RULE_flowControl = 8, 
		RULE_id = 9, RULE_array = 10, RULE_idarray = 11, RULE_any = 12;
	private static String[] makeRuleNames() {
		return new String[] {
			"main", "divide", "and_", "conquer", "unused", "unused2", "stat", "expr", 
			"flowControl", "id", "array", "idarray", "any"
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

	@Override
	public String getGrammarFileName() { return "TParser.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }


	/* public parser declarations/members section */
	bool myAction() { return true; }
	bool doesItBlend() { return true; }
	void cleanUp() {}
	void doInit() {}
	void doAfter() {}

	public TParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@SuppressWarnings("CheckReturnValue")
	public static class MainContext extends ParserRuleContext {
		public TerminalNode EOF() { return getToken(TParser.EOF, 0); }
		public List<StatContext> stat() {
			return getRuleContexts(StatContext.class);
		}
		public StatContext stat(int i) {
			return getRuleContext(StatContext.class,i);
		}
		public MainContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_main; }
	}

	public final MainContext main() throws RecognitionException {
		MainContext _localctx = new MainContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_main);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(27); 
			_errHandler.sync(this);
			_la = _input.LA(1);
			do {
				{
				{
				setState(26);
				stat();
				}
				}
				setState(29); 
				_errHandler.sync(this);
				_la = _input.LA(1);
			} while ( (((_la) & ~0x3f) == 0 && ((1L << _la) & 4259932L) != 0) );
			setState(31);
			match(EOF);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class DivideContext extends ParserRuleContext {
		public TerminalNode ID() { return getToken(TParser.ID, 0); }
		public And_Context and_() {
			return getRuleContext(And_Context.class,0);
		}
		public TerminalNode GreaterThan() { return getToken(TParser.GreaterThan, 0); }
		public DivideContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_divide; }
	}

	public final DivideContext divide() throws RecognitionException {
		DivideContext _localctx = new DivideContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_divide);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(33);
			match(ID);
			setState(37);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,1,_ctx) ) {
			case 1:
				{
				setState(34);
				and_();
				setState(35);
				match(GreaterThan);
				}
				break;
			}
			setState(39);
			if (!(doesItBlend())) throw new FailedPredicateException(this, "doesItBlend()");
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class And_Context extends ParserRuleContext {
		public TerminalNode And() { return getToken(TParser.And, 0); }
		public And_Context(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_and_; }
	}

	public final And_Context and_() throws RecognitionException {
		And_Context _localctx = new And_Context(_ctx, getState());
		enterRule(_localctx, 4, RULE_and_);
		 doInit(); 
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(41);
			match(And);
			}
			_ctx.stop = _input.LT(-1);
			 doAfter(); 
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ConquerContext extends ParserRuleContext {
		public Token ID;
		public List<DivideContext> divide() {
			return getRuleContexts(DivideContext.class);
		}
		public DivideContext divide(int i) {
			return getRuleContext(DivideContext.class,i);
		}
		public And_Context and_() {
			return getRuleContext(And_Context.class,0);
		}
		public TerminalNode ID() { return getToken(TParser.ID, 0); }
		public List<TerminalNode> LessThan() { return getTokens(TParser.LessThan); }
		public TerminalNode LessThan(int i) {
			return getToken(TParser.LessThan, i);
		}
		public ConquerContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_conquer; }
	}

	public final ConquerContext conquer() throws RecognitionException {
		ConquerContext _localctx = new ConquerContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_conquer);
		int _la;
		try {
			setState(63);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,5,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(44); 
				_errHandler.sync(this);
				_la = _input.LA(1);
				do {
					{
					{
					setState(43);
					divide();
					}
					}
					setState(46); 
					_errHandler.sync(this);
					_la = _input.LA(1);
				} while ( _la==ID );
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(48);
				if (!(doesItBlend())) throw new FailedPredicateException(this, "doesItBlend()");
				setState(49);
				and_();
				 myAction(); 
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				setState(52);
				((ConquerContext)_localctx).ID = match(ID);
				setState(60);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,4,_ctx) ) {
				case 1+1:
					{
					setState(56);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==LessThan) {
						{
						{
						setState(53);
						match(LessThan);
						}
						}
						setState(58);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					setState(59);
					divide();
					}
					break;
				}
				 (((ConquerContext)_localctx).ID!=null?((ConquerContext)_localctx).ID.getText():null); 
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class UnusedContext extends ParserRuleContext {
		public double input = 111;
		public double calculated;
		public int _a;
		public double _b;
		public int _c;
		public StatContext stat() {
			return getRuleContext(StatContext.class,0);
		}
		public UnusedContext(ParserRuleContext parent, int invokingState) { super(parent, invokingState); }
		public UnusedContext(ParserRuleContext parent, int invokingState, double input = 111) {
			super(parent, invokingState);
			this.input = input;
		}
		@Override public int getRuleIndex() { return RULE_unused; }
	}

	public final UnusedContext unused(double input = 111) throws RecognitionException {
		UnusedContext _localctx = new UnusedContext(_ctx, getState(), input);
		enterRule(_localctx, 8, RULE_unused);
		 doInit(); 
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(65);
			stat();
			}
			_ctx.stop = _input.LT(-1);
			 doAfter(); 
		}
		catch (...) {

			  // Replaces the standard exception handling.

		}
		finally {

			  cleanUp();

			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class Unused2Context extends ParserRuleContext {
		public List<TerminalNode> Semicolon() { return getTokens(TParser.Semicolon); }
		public TerminalNode Semicolon(int i) {
			return getToken(TParser.Semicolon, i);
		}
		public List<UnusedContext> unused() {
			return getRuleContexts(UnusedContext.class);
		}
		public UnusedContext unused(int i) {
			return getRuleContext(UnusedContext.class,i);
		}
		public TerminalNode Colon() { return getToken(TParser.Colon, 0); }
		public TerminalNode Plus() { return getToken(TParser.Plus, 0); }
		public Unused2Context(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_unused2; }
	}

	public final Unused2Context unused2() throws RecognitionException {
		Unused2Context _localctx = new Unused2Context(_ctx, getState());
		enterRule(_localctx, 10, RULE_unused2);
		int _la;
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(70); 
			_errHandler.sync(this);
			_alt = 1;
			do {
				switch (_alt) {
				case 1:
					{
					{
					setState(67);
					unused(1);
					setState(68);
					matchWildcard();
					}
					}
					break;
				default:
					throw new NoViableAltException(this);
				}
				setState(72); 
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,6,_ctx);
			} while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER );
			setState(75);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,7,_ctx) ) {
			case 1:
				{
				setState(74);
				_la = _input.LA(1);
				if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & 14336L) != 0)) ) {
				_errHandler.recoverInline(this);
				}
				else {
					if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
					_errHandler.reportMatch(this);
					consume();
				}
				}
				break;
			}
			setState(77);
			_la = _input.LA(1);
			if ( _la <= 0 || (_la==Semicolon) ) {
			_errHandler.recoverInline(this);
			}
			else {
				if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
				_errHandler.reportMatch(this);
				consume();
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class StatContext extends ParserRuleContext {
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode Equal() { return getToken(TParser.Equal, 0); }
		public TerminalNode Semicolon() { return getToken(TParser.Semicolon, 0); }
		public StatContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_stat; }
	}

	public final StatContext stat() throws RecognitionException {
		StatContext _localctx = new StatContext(_ctx, getState());
		enterRule(_localctx, 12, RULE_stat);
		try {
			setState(87);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,8,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(79);
				expr(0);
				setState(80);
				match(Equal);
				setState(81);
				expr(0);
				setState(82);
				match(Semicolon);
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(84);
				expr(0);
				setState(85);
				match(Semicolon);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ExprContext extends ParserRuleContext {
		public IdContext identifier;
		public TerminalNode OpenPar() { return getToken(TParser.OpenPar, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode ClosePar() { return getToken(TParser.ClosePar, 0); }
		public IdContext id() {
			return getRuleContext(IdContext.class,0);
		}
		public FlowControlContext flowControl() {
			return getRuleContext(FlowControlContext.class,0);
		}
		public TerminalNode INT() { return getToken(TParser.INT, 0); }
		public TerminalNode String() { return getToken(TParser.String, 0); }
		public TerminalNode Star() { return getToken(TParser.Star, 0); }
		public TerminalNode Plus() { return getToken(TParser.Plus, 0); }
		public TerminalNode QuestionMark() { return getToken(TParser.QuestionMark, 0); }
		public TerminalNode Colon() { return getToken(TParser.Colon, 0); }
		public TerminalNode Equal() { return getToken(TParser.Equal, 0); }
		public ExprContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_expr; }
	}

	public final ExprContext expr() throws RecognitionException {
		return expr(0);
	}

	private ExprContext expr(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		ExprContext _localctx = new ExprContext(_ctx, _parentState);
		ExprContext _prevctx = _localctx;
		int _startState = 14;
		enterRecursionRule(_localctx, 14, RULE_expr, _p);
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(98);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case OpenPar:
				{
				setState(90);
				match(OpenPar);
				setState(91);
				expr(0);
				setState(92);
				match(ClosePar);
				}
				break;
			case ID:
				{
				setState(94);
				((ExprContext)_localctx).identifier = id();
				}
				break;
			case Return:
			case Continue:
				{
				setState(95);
				flowControl();
				}
				break;
			case INT:
				{
				setState(96);
				match(INT);
				}
				break;
			case String:
				{
				setState(97);
				match(String);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
			_ctx.stop = _input.LT(-1);
			setState(117);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,11,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(115);
					_errHandler.sync(this);
					switch ( getInterpreter().adaptivePredict(_input,10,_ctx) ) {
					case 1:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(100);
						if (!(precpred(_ctx, 9))) throw new FailedPredicateException(this, "precpred(_ctx, 9)");
						setState(101);
						match(Star);
						setState(102);
						expr(10);
						}
						break;
					case 2:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(103);
						if (!(precpred(_ctx, 8))) throw new FailedPredicateException(this, "precpred(_ctx, 8)");
						setState(104);
						match(Plus);
						setState(105);
						expr(9);
						}
						break;
					case 3:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(106);
						if (!(precpred(_ctx, 6))) throw new FailedPredicateException(this, "precpred(_ctx, 6)");
						setState(107);
						match(QuestionMark);
						setState(108);
						expr(0);
						setState(109);
						match(Colon);
						setState(110);
						expr(6);
						}
						break;
					case 4:
						{
						_localctx = new ExprContext(_parentctx, _parentState);
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(112);
						if (!(precpred(_ctx, 5))) throw new FailedPredicateException(this, "precpred(_ctx, 5)");
						setState(113);
						match(Equal);
						setState(114);
						expr(5);
						}
						break;
					}
					} 
				}
				setState(119);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,11,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class FlowControlContext extends ParserRuleContext {
		public FlowControlContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_flowControl; }
	 
		public FlowControlContext() { }
		public void copyFrom(FlowControlContext ctx) {
			super.copyFrom(ctx);
		}
	}
	@SuppressWarnings("CheckReturnValue")
	public static class ReturnContext extends FlowControlContext {
		public TerminalNode Return() { return getToken(TParser.Return, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public ReturnContext(FlowControlContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class ContinueContext extends FlowControlContext {
		public TerminalNode Continue() { return getToken(TParser.Continue, 0); }
		public ContinueContext(FlowControlContext ctx) { copyFrom(ctx); }
	}

	public final FlowControlContext flowControl() throws RecognitionException {
		FlowControlContext _localctx = new FlowControlContext(_ctx, getState());
		enterRule(_localctx, 16, RULE_flowControl);
		try {
			setState(123);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case Return:
				_localctx = new ReturnContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(120);
				match(Return);
				setState(121);
				expr(0);
				}
				break;
			case Continue:
				_localctx = new ContinueContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(122);
				match(Continue);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class IdContext extends ParserRuleContext {
		public TerminalNode ID() { return getToken(TParser.ID, 0); }
		public IdContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_id; }
	}

	public final IdContext id() throws RecognitionException {
		IdContext _localctx = new IdContext(_ctx, getState());
		enterRule(_localctx, 18, RULE_id);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(125);
			match(ID);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ArrayContext extends ParserRuleContext {
		public Token INT;
		public List<Token> el = new ArrayList<Token>();
		public TerminalNode OpenCurly() { return getToken(TParser.OpenCurly, 0); }
		public TerminalNode CloseCurly() { return getToken(TParser.CloseCurly, 0); }
		public List<TerminalNode> INT() { return getTokens(TParser.INT); }
		public TerminalNode INT(int i) {
			return getToken(TParser.INT, i);
		}
		public List<TerminalNode> Comma() { return getTokens(TParser.Comma); }
		public TerminalNode Comma(int i) {
			return getToken(TParser.Comma, i);
		}
		public ArrayContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_array; }
	}

	public final ArrayContext array() throws RecognitionException {
		ArrayContext _localctx = new ArrayContext(_ctx, getState());
		enterRule(_localctx, 20, RULE_array);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(127);
			match(OpenCurly);
			setState(128);
			((ArrayContext)_localctx).INT = match(INT);
			((ArrayContext)_localctx).el.add(((ArrayContext)_localctx).INT);
			setState(133);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==Comma) {
				{
				{
				setState(129);
				match(Comma);
				setState(130);
				((ArrayContext)_localctx).INT = match(INT);
				((ArrayContext)_localctx).el.add(((ArrayContext)_localctx).INT);
				}
				}
				setState(135);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(136);
			match(CloseCurly);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class IdarrayContext extends ParserRuleContext {
		public IdContext id;
		public List<IdContext> element = new ArrayList<IdContext>();
		public TerminalNode OpenCurly() { return getToken(TParser.OpenCurly, 0); }
		public TerminalNode CloseCurly() { return getToken(TParser.CloseCurly, 0); }
		public List<IdContext> id() {
			return getRuleContexts(IdContext.class);
		}
		public IdContext id(int i) {
			return getRuleContext(IdContext.class,i);
		}
		public List<TerminalNode> Comma() { return getTokens(TParser.Comma); }
		public TerminalNode Comma(int i) {
			return getToken(TParser.Comma, i);
		}
		public IdarrayContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_idarray; }
	}

	public final IdarrayContext idarray() throws RecognitionException {
		IdarrayContext _localctx = new IdarrayContext(_ctx, getState());
		enterRule(_localctx, 22, RULE_idarray);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(138);
			match(OpenCurly);
			setState(139);
			((IdarrayContext)_localctx).id = id();
			((IdarrayContext)_localctx).element.add(((IdarrayContext)_localctx).id);
			setState(144);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==Comma) {
				{
				{
				setState(140);
				match(Comma);
				setState(141);
				((IdarrayContext)_localctx).id = id();
				((IdarrayContext)_localctx).element.add(((IdarrayContext)_localctx).id);
				}
				}
				setState(146);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(147);
			match(CloseCurly);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class AnyContext extends ParserRuleContext {
		public Token t;
		public AnyContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_any; }
	}

	public final AnyContext any() throws RecognitionException {
		AnyContext _localctx = new AnyContext(_ctx, getState());
		enterRule(_localctx, 24, RULE_any);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(149);
			((AnyContext)_localctx).t = matchWildcard();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public boolean sempred(RuleContext _localctx, int ruleIndex, int predIndex) {
		switch (ruleIndex) {
		case 1:
			return divide_sempred((DivideContext)_localctx, predIndex);
		case 3:
			return conquer_sempred((ConquerContext)_localctx, predIndex);
		case 7:
			return expr_sempred((ExprContext)_localctx, predIndex);
		}
		return true;
	}
	private boolean divide_sempred(DivideContext _localctx, int predIndex) {
		switch (predIndex) {
		case 0:
			return doesItBlend();
		}
		return true;
	}
	private boolean conquer_sempred(ConquerContext _localctx, int predIndex) {
		switch (predIndex) {
		case 1:
			return doesItBlend();
		}
		return true;
	}
	private boolean expr_sempred(ExprContext _localctx, int predIndex) {
		switch (predIndex) {
		case 2:
			return precpred(_ctx, 9);
		case 3:
			return precpred(_ctx, 8);
		case 4:
			return precpred(_ctx, 6);
		case 5:
			return precpred(_ctx, 5);
		}
		return true;
	}

	public static final String _serializedATN =
		"\u0004\u0001\u001f\u0098\u0002\u0000\u0007\u0000\u0002\u0001\u0007\u0001"+
		"\u0002\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0002\u0004\u0007\u0004"+
		"\u0002\u0005\u0007\u0005\u0002\u0006\u0007\u0006\u0002\u0007\u0007\u0007"+
		"\u0002\b\u0007\b\u0002\t\u0007\t\u0002\n\u0007\n\u0002\u000b\u0007\u000b"+
		"\u0002\f\u0007\f\u0001\u0000\u0004\u0000\u001c\b\u0000\u000b\u0000\f\u0000"+
		"\u001d\u0001\u0000\u0001\u0000\u0001\u0001\u0001\u0001\u0001\u0001\u0001"+
		"\u0001\u0003\u0001&\b\u0001\u0001\u0001\u0001\u0001\u0001\u0002\u0001"+
		"\u0002\u0001\u0003\u0004\u0003-\b\u0003\u000b\u0003\f\u0003.\u0001\u0003"+
		"\u0001\u0003\u0001\u0003\u0001\u0003\u0001\u0003\u0001\u0003\u0005\u0003"+
		"7\b\u0003\n\u0003\f\u0003:\t\u0003\u0001\u0003\u0003\u0003=\b\u0003\u0001"+
		"\u0003\u0003\u0003@\b\u0003\u0001\u0004\u0001\u0004\u0001\u0005\u0001"+
		"\u0005\u0001\u0005\u0004\u0005G\b\u0005\u000b\u0005\f\u0005H\u0001\u0005"+
		"\u0003\u0005L\b\u0005\u0001\u0005\u0001\u0005\u0001\u0006\u0001\u0006"+
		"\u0001\u0006\u0001\u0006\u0001\u0006\u0001\u0006\u0001\u0006\u0001\u0006"+
		"\u0003\u0006X\b\u0006\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007"+
		"\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0003\u0007"+
		"c\b\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007"+
		"\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007"+
		"\u0001\u0007\u0001\u0007\u0001\u0007\u0001\u0007\u0005\u0007t\b\u0007"+
		"\n\u0007\f\u0007w\t\u0007\u0001\b\u0001\b\u0001\b\u0003\b|\b\b\u0001\t"+
		"\u0001\t\u0001\n\u0001\n\u0001\n\u0001\n\u0005\n\u0084\b\n\n\n\f\n\u0087"+
		"\t\n\u0001\n\u0001\n\u0001\u000b\u0001\u000b\u0001\u000b\u0001\u000b\u0005"+
		"\u000b\u008f\b\u000b\n\u000b\f\u000b\u0092\t\u000b\u0001\u000b\u0001\u000b"+
		"\u0001\f\u0001\f\u0001\f\u0001<\u0001\u000e\r\u0000\u0002\u0004\u0006"+
		"\b\n\f\u000e\u0010\u0012\u0014\u0016\u0018\u0000\u0002\u0001\u0000\u000b"+
		"\r\u0001\u0000\f\f\u009f\u0000\u001b\u0001\u0000\u0000\u0000\u0002!\u0001"+
		"\u0000\u0000\u0000\u0004)\u0001\u0000\u0000\u0000\u0006?\u0001\u0000\u0000"+
		"\u0000\bA\u0001\u0000\u0000\u0000\nF\u0001\u0000\u0000\u0000\fW\u0001"+
		"\u0000\u0000\u0000\u000eb\u0001\u0000\u0000\u0000\u0010{\u0001\u0000\u0000"+
		"\u0000\u0012}\u0001\u0000\u0000\u0000\u0014\u007f\u0001\u0000\u0000\u0000"+
		"\u0016\u008a\u0001\u0000\u0000\u0000\u0018\u0095\u0001\u0000\u0000\u0000"+
		"\u001a\u001c\u0003\f\u0006\u0000\u001b\u001a\u0001\u0000\u0000\u0000\u001c"+
		"\u001d\u0001\u0000\u0000\u0000\u001d\u001b\u0001\u0000\u0000\u0000\u001d"+
		"\u001e\u0001\u0000\u0000\u0000\u001e\u001f\u0001\u0000\u0000\u0000\u001f"+
		" \u0005\u0000\u0000\u0001 \u0001\u0001\u0000\u0000\u0000!%\u0005\u0006"+
		"\u0000\u0000\"#\u0003\u0004\u0002\u0000#$\u0005\b\u0000\u0000$&\u0001"+
		"\u0000\u0000\u0000%\"\u0001\u0000\u0000\u0000%&\u0001\u0000\u0000\u0000"+
		"&\'\u0001\u0000\u0000\u0000\'(\u0004\u0001\u0000\u0000(\u0003\u0001\u0000"+
		"\u0000\u0000)*\u0005\n\u0000\u0000*\u0005\u0001\u0000\u0000\u0000+-\u0003"+
		"\u0002\u0001\u0000,+\u0001\u0000\u0000\u0000-.\u0001\u0000\u0000\u0000"+
		".,\u0001\u0000\u0000\u0000./\u0001\u0000\u0000\u0000/@\u0001\u0000\u0000"+
		"\u000001\u0004\u0003\u0001\u000012\u0003\u0004\u0002\u000023\u0006\u0003"+
		"\uffff\uffff\u00003@\u0001\u0000\u0000\u00004<\u0005\u0006\u0000\u0000"+
		"57\u0005\u0007\u0000\u000065\u0001\u0000\u0000\u00007:\u0001\u0000\u0000"+
		"\u000086\u0001\u0000\u0000\u000089\u0001\u0000\u0000\u00009;\u0001\u0000"+
		"\u0000\u0000:8\u0001\u0000\u0000\u0000;=\u0003\u0002\u0001\u0000<=\u0001"+
		"\u0000\u0000\u0000<8\u0001\u0000\u0000\u0000=>\u0001\u0000\u0000\u0000"+
		">@\u0006\u0003\uffff\uffff\u0000?,\u0001\u0000\u0000\u0000?0\u0001\u0000"+
		"\u0000\u0000?4\u0001\u0000\u0000\u0000@\u0007\u0001\u0000\u0000\u0000"+
		"AB\u0003\f\u0006\u0000B\t\u0001\u0000\u0000\u0000CD\u0003\b\u0004\u0000"+
		"DE\t\u0000\u0000\u0000EG\u0001\u0000\u0000\u0000FC\u0001\u0000\u0000\u0000"+
		"GH\u0001\u0000\u0000\u0000HF\u0001\u0000\u0000\u0000HI\u0001\u0000\u0000"+
		"\u0000IK\u0001\u0000\u0000\u0000JL\u0007\u0000\u0000\u0000KJ\u0001\u0000"+
		"\u0000\u0000KL\u0001\u0000\u0000\u0000LM\u0001\u0000\u0000\u0000MN\b\u0001"+
		"\u0000\u0000N\u000b\u0001\u0000\u0000\u0000OP\u0003\u000e\u0007\u0000"+
		"PQ\u0005\t\u0000\u0000QR\u0003\u000e\u0007\u0000RS\u0005\f\u0000\u0000"+
		"SX\u0001\u0000\u0000\u0000TU\u0003\u000e\u0007\u0000UV\u0005\f\u0000\u0000"+
		"VX\u0001\u0000\u0000\u0000WO\u0001\u0000\u0000\u0000WT\u0001\u0000\u0000"+
		"\u0000X\r\u0001\u0000\u0000\u0000YZ\u0006\u0007\uffff\uffff\u0000Z[\u0005"+
		"\u0010\u0000\u0000[\\\u0003\u000e\u0007\u0000\\]\u0005\u0011\u0000\u0000"+
		"]c\u0001\u0000\u0000\u0000^c\u0003\u0012\t\u0000_c\u0003\u0010\b\u0000"+
		"`c\u0005\u0004\u0000\u0000ac\u0005\u0016\u0000\u0000bY\u0001\u0000\u0000"+
		"\u0000b^\u0001\u0000\u0000\u0000b_\u0001\u0000\u0000\u0000b`\u0001\u0000"+
		"\u0000\u0000ba\u0001\u0000\u0000\u0000cu\u0001\u0000\u0000\u0000de\n\t"+
		"\u0000\u0000ef\u0005\u000f\u0000\u0000ft\u0003\u000e\u0007\ngh\n\b\u0000"+
		"\u0000hi\u0005\r\u0000\u0000it\u0003\u000e\u0007\tjk\n\u0006\u0000\u0000"+
		"kl\u0005\u0014\u0000\u0000lm\u0003\u000e\u0007\u0000mn\u0005\u000b\u0000"+
		"\u0000no\u0003\u000e\u0007\u0006ot\u0001\u0000\u0000\u0000pq\n\u0005\u0000"+
		"\u0000qr\u0005\t\u0000\u0000rt\u0003\u000e\u0007\u0005sd\u0001\u0000\u0000"+
		"\u0000sg\u0001\u0000\u0000\u0000sj\u0001\u0000\u0000\u0000sp\u0001\u0000"+
		"\u0000\u0000tw\u0001\u0000\u0000\u0000us\u0001\u0000\u0000\u0000uv\u0001"+
		"\u0000\u0000\u0000v\u000f\u0001\u0000\u0000\u0000wu\u0001\u0000\u0000"+
		"\u0000xy\u0005\u0002\u0000\u0000y|\u0003\u000e\u0007\u0000z|\u0005\u0003"+
		"\u0000\u0000{x\u0001\u0000\u0000\u0000{z\u0001\u0000\u0000\u0000|\u0011"+
		"\u0001\u0000\u0000\u0000}~\u0005\u0006\u0000\u0000~\u0013\u0001\u0000"+
		"\u0000\u0000\u007f\u0080\u0005\u0012\u0000\u0000\u0080\u0085\u0005\u0004"+
		"\u0000\u0000\u0081\u0082\u0005\u0015\u0000\u0000\u0082\u0084\u0005\u0004"+
		"\u0000\u0000\u0083\u0081\u0001\u0000\u0000\u0000\u0084\u0087\u0001\u0000"+
		"\u0000\u0000\u0085\u0083\u0001\u0000\u0000\u0000\u0085\u0086\u0001\u0000"+
		"\u0000\u0000\u0086\u0088\u0001\u0000\u0000\u0000\u0087\u0085\u0001\u0000"+
		"\u0000\u0000\u0088\u0089\u0005\u0013\u0000\u0000\u0089\u0015\u0001\u0000"+
		"\u0000\u0000\u008a\u008b\u0005\u0012\u0000\u0000\u008b\u0090\u0003\u0012"+
		"\t\u0000\u008c\u008d\u0005\u0015\u0000\u0000\u008d\u008f\u0003\u0012\t"+
		"\u0000\u008e\u008c\u0001\u0000\u0000\u0000\u008f\u0092\u0001\u0000\u0000"+
		"\u0000\u0090\u008e\u0001\u0000\u0000\u0000\u0090\u0091\u0001\u0000\u0000"+
		"\u0000\u0091\u0093\u0001\u0000\u0000\u0000\u0092\u0090\u0001\u0000\u0000"+
		"\u0000\u0093\u0094\u0005\u0013\u0000\u0000\u0094\u0017\u0001\u0000\u0000"+
		"\u0000\u0095\u0096\t\u0000\u0000\u0000\u0096\u0019\u0001\u0000\u0000\u0000"+
		"\u000f\u001d%.8<?HKWbsu{\u0085\u0090";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}