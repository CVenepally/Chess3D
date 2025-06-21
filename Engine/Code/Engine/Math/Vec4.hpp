#pragma once



struct Vec4
{

public: 

	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

	static const Vec4 ZERO;
	static const Vec4 ONE;

public:

	~Vec4() {}												                
	Vec4() {}												                
	
	Vec4( Vec4 const& copyFrom );							                
	
	explicit Vec4 (float initialX, float initialY, float initialZ, float initialW );		

	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetAngleAboutZRadians() const;
	float GetAngleAboutZDegrees() const;
	
	Vec4 const GetRotatedAboutZRadians(float deltaRadians) const;
	Vec4 const GetRotatedAboutZDegrees(float deltaDegrees) const;
	Vec4 const GetClamped(float maxLength) const;
	Vec4 const GetNormalized() const;

	void SetFromText(char const* textVec4);

	bool		operator==( Vec4 const& compare ) const;		
	bool		operator!=( Vec4 const& compare ) const;		
	
	Vec4 const	operator+( Vec4 const& vecToAdd ) const;
	Vec4 const	operator-( Vec4 const& vecToSubtract ) const;	
	Vec4 const	operator-() const;								
	Vec4 const	operator*( float uniformScale ) const;			
	Vec4 const	operator*( Vec4 const& vecToMultiply ) const;	
	Vec4 const	operator/( float inverseScale ) const;			

	
	void		operator+=( Vec4 const& vecToAdd );				
	void		operator-=( Vec4 const& vecToSubtract );		
	void		operator*=( const float uniformScale );			
	void		operator/=( const float uniformDivisor );		
	void		operator=( Vec4 const& copyFrom );				

	friend Vec4 const operator*( float uniformScale, Vec4 const& vecToScale );	
};


