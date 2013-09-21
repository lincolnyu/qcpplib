//
//  qcpplib v1.00
//  quanben's C++ template library
//  
//  Author Lincoln Yu
//
//  lincoln.yu@gmail.com
//  https://github.com/lincolnyu/qcpplib
//
//  The MIT License (MIT)
// 
//  Copyright (c) <year> <copyright holders>
// 
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
// 
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
// 
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#if !defined (_WILDCARD_H_)
#define _WILDCARD_H_

#include <map>
#include <vector>
#include <cstring>
#include <string>

/// @brief Contains class definitions that deal with wildcard matching
namespace Qtl { namespace String { namespace Wildcard {

    /// @brief An implementation of the functor that returns the length of string whose iterators are applicable 
    ///        to subtract operator
    template <class TStringRef, class TSubtractableIter>
    struct CharDistFunctorIndexed
    {
        size_t operator()(TStringRef str, TSubtractableIter iterBegin, TSubtractableIter iterEnd)
        {
            return (iterEnd-iterBegin);
        }
    };
    
    /// @brief An implementation of the functor that returns the position of the first character for character-based 
    ///        zero-terminated string
    struct StringBeginFunctorPsz
    {
        char * operator()(char *str)
        {
            return (str);
        }
    };

    /// @brief An implementation of the functor that returns the position of the first character for a std::string
    struct StringBeginFunctorStdStr
    {
        std::string::const_iterator operator()(const std::string& str)
        {
            return str.begin();
        }
    };
    
    /// @brief An implementation of the functor that determines if the position is at the end of a character-based
    ///        zero-terminated string
    struct StringEndFunctorPsz
    {
        bool operator()(char *iter, char *str)
        {
            return (*iter == 0);
        }
    };

    /// @brief An implementation of the functor that determines if the position is at the end of a std::string
    struct StringEndFunctorStdStr
    {
        bool operator()(std::string::const_iterator iter, const std::string& str)
        {
            return (iter==str.end());
        }
    };

    /// @brief An implementation of the functor that appends a character to a character-based zero-terminated string
    struct AppendCharFunctorPsz
    {
        void operator()(char *str, char *&iter, char ch)
        {
            *iter++ = ch;
        }
    };

    /// @brief An implementation of the functor that appends a character to a std::string
    struct AppendCharFunctorStdStr
    {
        void operator()(std::string &str, std::string::iterator &iter, char ch)
        {
            str.push_back(ch);
        }
    };

    /// @brief The default class that provides string functors
    struct DefaultStringFunctorSelector
    {
        /// @brief The generic rebinder
        template <class TStringRef, class TCharIter>
        struct rebind
        {
            // unimplemented, compiler error occurs if getting here
        };
        
        /// @brief The rebinder to the character array based string functors
        template <>
        struct rebind<char*,char*>
        {
            typedef StringBeginFunctorPsz               StringBeginFunctor;
            typedef StringEndFunctorPsz             StringEndFunctor;
            typedef CharDistFunctorIndexed<char*,char*> CharDistFunctor;
        };

        /// @brief The rebinder to the std::string based string functors
        template <>
        struct rebind<const std::string&, std::string::const_iterator>
        {
            typedef StringBeginFunctorStdStr            StringBeginFunctor;
            typedef StringEndFunctorStdStr              StringEndFunctor;
            typedef CharDistFunctorIndexed<std::string::const_iterator, std::string::const_iterator>    CharDistFunctor;
        };
    };

    /// @brief The default class that provides functor that appends character to string
    struct DefaultAppendCharFunctorSelector
    {
        /// @brief The generic binder
        template <class TStringRef, class TCharIter, class TChar>
        struct rebind
        {
            // unimplemented, compiler error occurs if getting here
        };

        /// @brief The rebinder to the functor that appends character to character-based zero-terminating string
        template <>
        struct rebind<char*, char*, char>
        {
            typedef AppendCharFunctorPsz    AppendCharFunctor;
        };

        /// @brief The rebinder to the functor that appends character to std::string
        template <>
        struct rebind<std::string&, std::string::iterator, char>
        {
            typedef AppendCharFunctorStdStr AppendCharFunctor;
        };
    };
    
    /// @brief A class that encapsulates a wildcard pattern
    /// @param TString The type of the pattern string
    /// @param TStringRef The type of the reference to the pattern string (for efficient parameter passing)
    /// @param TCharIter The type of the iterator through the characters
    /// @param TStringBeginFunctor The type of the functor that returns the iterator at the beginning of a string
    /// @param TStringEndFunctor The type of the functor that determines if the iterator is at the end of a string
    template <class TString=char*, class TStringRef=char*, class TCharIter=char*,
        class TStringFunctorSelector=DefaultStringFunctorSelector>
    class Pattern
    {
    public:
        typedef TStringRef StringRef;

        /// @brief The type of iterator through the characters in the pattern string
        typedef TCharIter CharIter;
        
        /// @brief The type of the functor that returns the iterator at the beginning of a string
        typedef typename TStringFunctorSelector::template rebind<TStringRef, TCharIter>::StringBeginFunctor StringBeginFunctor;
        
        /// @brief The type of the functor that determines if the iterator is at the end of a string
        typedef typename TStringFunctorSelector::template rebind<TStringRef, TCharIter>::StringEndFunctor   StringEndFunctor;

        /// @brief The type of the functor that returns the distance between two characters 
        typedef typename TStringFunctorSelector::template rebind<TStringRef, TCharIter>::CharDistFunctor CharDistFunctor;

    private:
        /// @brief The pattern string
        TString                 _pattern;
        
        /// @brief The functor that returns the beginning of the string
        StringBeginFunctor      _getStringBegin;
        
        /// @brief The functor that returns if the iterator is at the end of the string
        StringEndFunctor        _isStringEnd;

        /// @brief The functor that returns the distance between two characters
        CharDistFunctor         _getCharDist;
        
        /// @brief The look-up table that maps iterator of pattern to the index of match result entry
        std::map<CharIter, int> _mapIterToIndex;

    public:
        // a typical wildcard pattern: 
        //   a*b?C(*)
        // 
        /// @brief Instantiates a pattern with the pattern string and the functors
        /// @param pattern The pattern string
        /// @param stringBegin The functor that provides the beginning of the string
        /// @param stringEnd The functor that determines the end of the string
        /// @remarks A typical wildcard pattern is like: a*b?C(*)D\)
        ///          where normal characters (alphanumerics, punctuation etc) expect exact match, asteroids match whatever 
        ///          string of whatever length, question marks match any single character and an escape character 
        ///          (back-slash) turns a succeeding special character to a normal matching character.
        Pattern(TStringRef pattern, StringBeginFunctor stringBegin, StringBeginFunctor stringEnd) 
            : _pattern(pattern), _getStringBegin(stringBegin), _isStringEnd(stringEnd)
        {
            PreProcessParentheses();
        }

        /// @brief Instantiates a pattern with the pattern string
        /// @param pattern The pattern string
        Pattern(TStringRef pattern) : _pattern(pattern)
        {
            PreProcessParentheses();
        }

    private:
        /// @brief Creates the mapping from parenthesis pointer to index from the pattern string
        void PreProcessParentheses()
        {
            _mapIterToIndex.clear();
            int openingIndex = 0;
            int closingIndex = 0;
            for (CharIter iter = GetBegin(); !IsEnd(iter); ++iter)
            {
                if (*iter=='\\')
                {
                    ++iter; // skip the character that follows
                }
                else if (*iter == '(')
                {
                    _mapIterToIndex[iter] = closingIndex = openingIndex++;
                }
                else if (*iter == ')')
                {
                    // NOTE We don't need to differentiate opening and closing parentheses as
                    //      the matcher has the knowledge of the pattern characters
                    _mapIterToIndex[iter] = closingIndex--;
                }
            }
        }

    public:
        /// @brief Returns the beginning of the pattern string
        /// @return The iterator point to the beginning of the pattern string
        CharIter GetBegin()
        {
            return _getStringBegin(_pattern);
        }

        /// @brief Determines if the iterator is at the end of the pattern string
        /// @param The interator in question
        /// @return true if the iterator is at the beginning of the pattern string
        bool IsEnd(CharIter iter)
        {
            return _isStringEnd(iter, _pattern);
        }

        /// @brief Returns the match entry index for the specified parenthesis pointer
        /// @return The match entry index
        int PatternIterToIndex(CharIter patternIter)
        {
            return _mapIterToIndex[patternIter];
        }

        /// @brief Returns the distance between two characters (the number of characters in between plus one)
        /// @param iterBegin The iterator that points to the character on the left hand
        /// @param iterEnd The iterator that points to the character on the right hand
        /// @return The distance
        size_t GetQuotedLength(CharIter iterBegin, CharIter iterEnd)
        {
            return _getCharDist(_pattern, iterBegin, iterEnd);
        }
    };

    /// @brief A class that converts a wildcard pattern to its equivalent regular expression
    /// @param TPattern The type of the pattern class 
    /// @param TRegexStringRef The type of the reference to the string for regular expression
    /// @param TRegexCharIter The iterator through characters in the string for regular expression
    /// @param TPatternFunctorSelector The functor selector for pattern
    /// @param TRegexAppendCharFunctorSelector The append-character functor selector for regular expression
    /// @remarks NOTE TRegexChar has to be compatible with the character type TPattern::CharIter iterates through
    template <class TPattern=Pattern<>, class TRegexStringRef=char*, class TRegexCharIter=char*, 
        class TRegexChar=char, class TRegexAppendCharFunctorSelector=DefaultAppendCharFunctorSelector>
    class WildCardToRegex
    {
    public:
        /// @brief The type of the reference to regular expression string
        typedef TRegexStringRef     RegexStringRef;
        /// @brief The type of the iteartor through the characters in the regular expression string
        typedef TRegexCharIter      RegexCharIter;
        /// @brief The type of the character that can be append to the regular expression string
        typedef TRegexChar          RegexChar;

        /// @brief The type of the reference to the wildcard string
        typedef typename TPattern::StringRef    PatternStringRef;
        /// @brief The type of the iterator through the characters in the wildcard string
        typedef typename TPattern::CharIter     PatternStringIter;

        /// @brief
        typedef typename TRegexAppendCharFunctorSelector::template rebind<RegexStringRef, RegexCharIter, RegexChar>::AppendCharFunctor
            RegexAppendCharFunctor;

    private:
        /// @brief The functor that appends character to the regular expression string
        RegexAppendCharFunctor _regexAppendChar;

    public:
        /// @brief Initialises a WildCardToRegex with the specified functor instances
        /// @param regexAppendChar The functor that appends character to the regular expression string
        WildCardToRegex(RegexAppendCharFunctor &regexAppendChar)
            : _regexAppendChar(regexAppendChar)
        {
        }

        /// @brief Initialises a WildCardToRegex with the default settings
        WildCardToRegex()
        {
        }

    public:
        /// @brief Converts a wildcard string to its equivalent regular expression
        /// @remarks This is supposed to comply with the rules set by the regex implementation in QSharp
        ///          See https://qsharp.codeplex.com/SourceControl/latest#QSharp/QSharp.String.Rex/Creator.cs
        ///          for more detail. It has yet to be tested though.
        void Convert(TPattern &pattern, TRegexStringRef regex, TRegexCharIter iterRegex)
        {
            for (PatternStringIter iter = pattern.GetBegin(); !pattern.IsEnd(iter); ++iter)
            {
                switch (*iter)
                {
                case '\\':
                    _regexAppendChar(regex, iterRegex, *iter);
                    ++iter;
                    if (!pattern.IsEnd(iter))
                    {
                        _regexAppendChar(regex, iterRegex, *iter);
                    }
                    else
                    {
                        _regexAppendChar(regex, iterRegex, '\\');
                    }
                    break;
                case '*':
                    _regexAppendChar(regex, iterRegex, '.');
                    _regexAppendChar(regex, iterRegex, *iter);
                    break;
                case '?':
                    _regexAppendChar(regex, iterRegex, '.');
                    break;
                case '(': case ')':
                    _regexAppendChar(regex, iterRegex, *iter);
                    break;
                case '[': case ']': case '{': case '}': case '^': case '.': case '-': case '+':
                    _regexAppendChar(regex, iterRegex, '\\');
                    _regexAppendChar(regex, iterRegex, *iter);
                    break;
                default:
                    _regexAppendChar(regex, iterRegex, *iter);
                    break;
                }
            }
        }
    };

    /// @brief A class that represents a match of quotation enclosed by a pair of parentheses in the pattern
    /// @param TCharIter The type of iterator through the source string
    /// @param TDiff The type of a integer number that indicates the length of string or the distance between characters
    template <class TCharIter=char*, class TDiff=size_t>
    class MatchQuote
    {
    public:
        /// @brief The type of iterator through the source string
        typedef TCharIter   CharIter;

        /// @brief The type of a integer number that indicates the length of string or the distance between characters
        typedef TDiff       Diff;

    public:
        /// @brief The beginning of the substring that matches
        CharIter Begin;

        /// @brief The end of the substring that matches
        CharIter End;
    };  
    
    /// @brief A class that contains all the matched quotations
    /// @param TCharIter The iterator through the source string
    /// @param TDiff The type of the integer that indicates a string length or a character distance
    template <class TCharIter=char*, class TDiff=size_t>
    class MatchResult
    {
    public:
        /// @brief The iterator through the source string
        typedef TCharIter       CharIter;
        /// @brief The type of the integer that indicates a string length or a character distance
        typedef TDiff           Diff;
        /// @brief The type of match entries listed in this object
        typedef MatchQuote<CharIter, Diff>  MatchType;

    public:
        /// @brief A list of matched quotation entries
        std::vector<MatchType> Matches;

    public:
        /// @brief Records the beginning of a quotation encountered
        /// @param index The index of the match entry
        /// @param iterChar The pointer to the source string where the quotation starts
        void Open(int index, CharIter iterChar)
        {
            while (index >= Matches.size())
            {
                Matches.push_back(MatchType());
            }
            Matches[index].Begin = iterChar;
        }
        
        /// @brief Records the end of a quotation encountered
        /// @param index The index of the match entry
        /// @param iterChar The pointer to the source string where the quotation ends
        void Close(int index, CharIter iterChar)
        {
            // cell index must have already been allocated in the array of Matches
            Matches[index].End = iterChar;
        }
    };

    /// @brief A default trait class that provides types needed by Matcher
    /// @param TChar 
    template <class TStringRef=char*, class TCharIter=char*, class TDiff=size_t,
        class TStringFunctorSelector=DefaultStringFunctorSelector>
    struct MatcherTraits
    {
        /// @brief The type of the reference to the source string 
        typedef TStringRef  StringRef;
        /// @brief The type of iterator through the characters in the source string
        typedef TCharIter   CharIter;
        
        /// @brief The type of the match result (matched quotation entry container)
        typedef MatchResult<TCharIter, TDiff>   MatchResultType;
        /// @brief The type of the reference to the match result
        typedef MatchResultType &               MatchResultRef;
        
        /// @brief The type of the functor that returns the beginning of a string
        typedef typename TStringFunctorSelector::template rebind<StringRef, CharIter>::StringBeginFunctor   StringBeginFunctor;
        /// @brief The type of the functor that determines if an iterator is at the end of a string
        typedef typename TStringFunctorSelector::template rebind<StringRef, CharIter>::StringEndFunctor     StringEndFunctor;
    };
    
    /// @brief A wildcard string matcher
    template <class Traits = MatcherTraits<>>
    class Matcher
    {
    public:
        /// @brief The type of the reference to the source string 
        typedef typename Traits::StringRef          StringRef;
        /// @brief The type of iterator through the characters in the source string
        typedef typename Traits::CharIter           CharIter;
        
        /// @brief The type of the reference to the match result
        typedef typename Traits::MatchResultRef     MatchResultRef;
        
        /// @brief The type of the functor that returns the beginning of a string
        typedef typename Traits::StringBeginFunctor StringBeginFunctor;
        /// @brief The type of the functor that determines if an iterator is at the end of a string
        typedef typename Traits::StringEndFunctor   StringEndFunctor;

    private:
        /// @brief The functor that returns the beginning of a string
        StringBeginFunctor  _stringBegin;

        /// @brief The functor that determines if an iterator is at the end of a string
        StringEndFunctor    _stringEnd;
                
    public:
        /// @brief Instantiates a Matcher with the specified string functor instances
        /// @param stringBegin The functor that returns the beginning of a string
        /// @param stringEnd The functor that determines if an iterator is at the end of a string
        Matcher(StringBeginFunctor &stringBegin, StringEndFunctor &stringEnd)
            : _stringBegin(stringBegin), _stringEnd(stringEnd)
        {
        }
        
        /// @brief Instantiates a Matcher with default settings
        Matcher()
        {
        }
        
    public:
        /// @brief Match The source to the pattern
        /// @param source The source string to match
        /// @param pattern The pattern to match against
        /// @param matchResult The container of matched quotation entries
        /// @return true if the matching is successful (the pattern is completely consumed)
        template <class TPattern>
        bool Match(StringRef source, TPattern &pattern, MatchResultRef matchResult)
        {
            CharIter iterSource = _stringBegin(source);
            TPattern::CharIter iterPattern = pattern.GetBegin();
            return Match(source, iterSource, pattern, iterPattern, matchResult);
        }
        
        /// @brief Match the source to the pattern (recursive)
        /// @param source The source string to match
        /// @param iterSource The iterator through the source string at its current position
        /// @param pattern The pattern to match against
        /// @param iterPattern The iterator through the pattern string at its current position
        /// @param matchResult The container of matched quotation entries
        /// @return true if the matching is successful (the pattern is completely consumed)
        template <class TPattern>
        bool Match(StringRef source, CharIter &iterSource, TPattern &pattern, typename TPattern::CharIter &iterPattern, 
            MatchResultRef matchResult)
        {
            while (! pattern.IsEnd(iterPattern))
            {
                if (*iterPattern == '\\')
                {
                    ++iterPattern;
                }
                else if (*iterPattern == '*')
                {
                    CharIter savedIterSource = iterSource;
                    TPattern::CharIter savedIterPattern = iterPattern;
                    // greedy strategy
                    if (!_stringEnd(savedIterSource, source))
                    {
                        ++iterSource;
                        if (Match(source, iterSource, pattern, savedIterPattern, matchResult))
                        {
                            return true;
                        }
                    }
                    ++iterPattern;
                    if (Match(source, savedIterSource, pattern, iterPattern, matchResult))
                    {
                        return true;
                    }
                    return false;
                }
                else if (*iterPattern == '?')
                {
                    if (_stringEnd(iterSource, source))
                    {
                        return false;
                    }
                    ++iterPattern;
                    ++iterSource;
                    continue;
                }
                else if (*iterPattern == '(')
                {
                    int index = pattern.PatternIterToIndex(iterPattern);
                    matchResult.Open(index, iterSource);
                    ++iterPattern;
                    continue;
                }
                else if (*iterPattern == ')')
                {
                    int index = pattern.PatternIterToIndex(iterPattern);
                    matchResult.Close(index, iterSource);
                    ++iterPattern;
                    continue;
                }
                
                if (!_stringEnd(iterSource, source) && *iterPattern == *iterSource)
                {
                    ++iterPattern;
                    ++iterSource;
                }
                else
                {
                    return false;
                }
            }
            return true;
        }
    };
}}}

#endif
