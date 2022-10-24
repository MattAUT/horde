#ifndef __ROUND_H__
#define __ROUND_H__

// Forward Declarations

class Round
{
	// Member Methods
public:
	Round();
	~Round();

	void Increment();
	unsigned int GetCurrentRound() const;
	float GetSpawnDelay() const;
	float GetSpawnHealth() const;
	unsigned int GetSpawnCount() const;

protected:
	void SetParameters();

private:
	Round(const Round& s);

	// Member Data
public:
protected:
	unsigned int m_uiRoundCount;
	unsigned int m_uiSpawnCount;
	float m_fSpawnHealth;
	float m_fSpawnDelay;
private:

};

#endif // __ROUND_H__

