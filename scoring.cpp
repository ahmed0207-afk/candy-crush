// This function calculates the new score after candies are cleared
int calculateScore(int currentScore, int candiesCleared)
{
    int pointsEarned = candiesCleared * 10;

    return currentScore + pointsEarned;
}