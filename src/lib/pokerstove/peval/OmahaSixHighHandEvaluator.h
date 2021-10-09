/**
 * Copyright (c) 2012 Andrew Prock. All rights reserved.
 * $Id: OmahaSixHighHandEvaluator.h 2649 2012-06-30 04:53:24Z prock $
 */
#ifndef PEVAL_OMAHASIXHIGHHANDEVALUATOR_H_
#define PEVAL_OMAHASIXHIGHHANDEVALUATOR_H_

#include "Holdem.h"
#include "PokerEvaluationTables.h"
#include "PokerHandEvaluator.h"
#include <boost/math/special_functions/binomial.hpp>

namespace pokerstove
{
/**
 * A specialized hand evaluator for six card omaha.  Not as slow.
 */
class OmahaSixHighHandEvaluator : public PokerHandEvaluator
{
public:
    static const int NUM_OMAHA_POCKET = 6;
    static const int NUM_OMAHA_FLOP = 3;
    static const int NUM_OMAHA_RIVER = 5;
    static const int NUM_OMAHA_HAND_USE = 2;
    static const int NUM_OMAHA_FLUSH_BOARD = 3;

    virtual PokerHandEvaluation evaluateHand(const CardSet& hand, const CardSet& board) const
    {
        PokerEvaluation eval[2];

        // generate the possible sub hands, player hand candidates are all
        // Nc2 combinations of hands cards N - number of cards in pocket hand.
        // board candidates are all Nc3 board candidates, where N is the size
        // of the board
        double combos = boost::math::binomial_coefficient<double>(board.size(), 3);
        double pockets = boost::math::binomial_coefficient<double>(hand.size(), 2);

        std::vector<CardSet> board_candidates(static_cast<size_t>(combos));
        std::vector<CardSet> hand_candidates(static_cast<size_t>(pockets));
        fillHands(hand_candidates, hand);
        fillBoards(board_candidates, board);

        for (size_t i = 0; i < hand_candidates.size(); i++)
            for (size_t j = 0; j < board_candidates.size(); j++)
            {
                PokerEvaluation e = CardSet(hand_candidates[i] | board_candidates[j]).evaluateHigh();
                if (e > eval[0])
                    eval[0] = e;
            }

        return PokerHandEvaluation(eval[0]);
    }

    virtual PokerEvaluation evaluateRanks(const CardSet& hand,
                                          const CardSet& board) const
    {
        PokerEvaluation eval;

        // generate the possible sub hands, player hand candidates are all
        // Nc2 combinations of hands cards N - number of cards in pocket hand.
        // board candidates are all Nc3 board candidates, where N is the size
        // of the board
        double combos = boost::math::binomial_coefficient<double>(board.size(), 3);
        double pockets = boost::math::binomial_coefficient<double>(hand.size(), 2);

        std::vector<CardSet> board_candidates(static_cast<size_t>(combos));
        std::vector<CardSet> hand_candidates(static_cast<size_t>(pockets));
        fillHands(hand_candidates, hand);
        fillBoards(board_candidates, board);
        for (size_t i = 0; i < hand_candidates.size(); i++)
            for (size_t j = 0; j < board_candidates.size(); j++)
            {
                CardSet meld(hand_candidates[i]);
                meld.insertRanks(board_candidates[j]);
                PokerEvaluation e = meld.evaluateHighRanks();
                if (e > eval)
                    eval = e;
            }

        return eval;
    }

    virtual PokerEvaluation evaluateSuits(const CardSet& hand, const CardSet& board) const
    {
        PokerEvaluation eval;

        // generate the possible sub hands, player hand candidates are all
        // Nc2 combinations of hands cards N - number of cards in pocket hand.
        // board candidates are all Nc3 board candidates, where N is the size
        // of the board
        double combos = boost::math::binomial_coefficient<double>(board.size(), 3);
        double pockets = boost::math::binomial_coefficient<double>(hand.size(), 2);

        std::vector<CardSet> board_candidates(static_cast<size_t>(combos));
        std::vector<CardSet> hand_candidates(static_cast<size_t>(pockets));
        fillHands(hand_candidates, hand);
        fillBoards(board_candidates, board);

        for (size_t i = 0; i < hand_candidates.size(); i++)
            for (size_t j = 0; j < board_candidates.size(); j++)
            {
                PokerEvaluation e =
                    CardSet(hand_candidates[i] | board_candidates[j])
                        .evaluateHighFlush();
                if (e > eval)
                    eval = e;
            }

        return eval;
    }

    void fillHands(std::vector<CardSet>& candidates, const CardSet& cards) const
    {
        std::vector<CardSet> clist = cards.cardSets();
        int cand_i = 0;
        for (size_t i = 0; i < cards.size(); i++)
            for (size_t j = i+1; j < cards.size(); j++)
            {
                candidates[cand_i] = (clist[i] | clist[j]);
                cand_i += 1;
            }
    }

    void fillBoards(std::vector<CardSet>& candidates,
                    const CardSet& cards) const
    {
        std::vector<CardSet> clist = cards.cardSets();
        switch (clist.size())
        {
            case 5:
                candidates[4] = (clist[0] | clist[1] | clist[4]);
                candidates[5] = (clist[0] | clist[2] | clist[4]);
                candidates[6] = (clist[0] | clist[3] | clist[4]);
                candidates[7] = (clist[1] | clist[2] | clist[4]);
                candidates[8] = (clist[1] | clist[3] | clist[4]);
                candidates[9] = (clist[2] | clist[3] | clist[4]);
                // fall through

            case 4:
                candidates[1] = (clist[0] | clist[1] | clist[3]);
                candidates[2] = (clist[0] | clist[2] | clist[3]);
                candidates[3] = (clist[1] | clist[2] | clist[3]);
                // fall through

            case 3:
                candidates[0] = (clist[0] | clist[1] | clist[2]);
        }
    }

    virtual size_t handSize() const { return NUM_OMAHA_POCKET; }
    virtual size_t boardSize() const { return BOARD_SIZE; }
    virtual size_t evaluationSize() const { return 1; }
};

}  // namespace pokerstove
#endif  // PEVAL_OMAHASIXHIGHHANDEVALUATOR_H_
