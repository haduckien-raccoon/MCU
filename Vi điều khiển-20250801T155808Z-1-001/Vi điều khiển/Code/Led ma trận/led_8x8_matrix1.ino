#define ROW1 13
#define ROW2 12
#define ROW3 11
#define ROW4 10
#define ROW5 9
#define ROW6 8
#define ROW7 7
#define ROW8 6

#define COL1 5
#define COL2 4
#define COL3 3
#define COL4 2
#define COL5 A4
#define COL6 A3
#define COL7 A2
#define COL8 A1

const int row[] = {ROW1, ROW2, ROW3, ROW4, ROW5, ROW6, ROW7, ROW8};
const int col[] = {COL1,COL2, COL3, COL4, COL5, COL6, COL7, COL8};

int K[8][8] = {{1,1,1,1,1,1,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,0,1,0,1,1,1},
		       {1,1,0,0,1,1,1,1},
		       {1,1,0,0,1,1,1,1},
		       {1,1,0,1,0,1,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,1,1,1,1,1,1}};

int H[8][8] = {{1,1,1,1,1,1,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,0,0,0,0,1,1},
		       {1,1,0,0,0,0,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,1,1,1,1,1,1}};

int A[8][8] = {{1,1,1,1,1,1,1,1},
		       {1,1,1,0,0,1,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,0,0,0,0,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,0,1,1,0,1,1},
		       {1,1,1,1,1,1,1,1}};

int I[8][8] = {{1,1,1,1,1,1,1,1},
	           {1,1,0,0,0,0,1,1},
	           {1,1,1,0,0,1,1,1},
		       {1,1,1,0,0,1,1,1},
		       {1,1,1,0,0,1,1,1},
		       {1,1,1,0,0,1,1,1},
		       {1,1,0,0,0,0,1,1},
		       {1,1,1,1,1,1,1,1}};

void setup() {
  Serial.begin(9600);
  for (int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  pinMode(A1, OUTPUT);
  digitalWrite(A1, LOW);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, LOW);
  pinMode(A3, OUTPUT);
  digitalWrite(A3, LOW);
  pinMode(A4, OUTPUT);
  digitalWrite(A4, LOW);
}

void loop() {
  yaz(K);
  delay(2000);
  yaz(H);
  delay(2000);
  yaz(A);
  delay(2000);
  yaz(I);
  delay(2000);
}

void  yaz(int matrix[8][8]){
  // col[c] to set (+) of led
  // row[c] to set (-) of led
  for (int c=0; c < 8; c++){

    // Set each col is HIGH, (+) of led HIGH
    digitalWrite(col[c], HIGH);
    for (int r = 0; r < 8; r++){
      // IF matrix[r][c] is 0 => row[r] = LOW, (-) of led LOW
      // else row[r] = HIGH, both sides of led are HIGH => led turn off
      digitalWrite(row[r], HIGH*matrix[r][c]); 
      delay(1);
    }
    
    // Return default value
    for (int r = 0; r < 8; r++){
      digitalWrite(row[r], HIGH);
      delay(1);
    }
  
    // Return default value
    digitalWrite(col[c], LOW);
  }
}