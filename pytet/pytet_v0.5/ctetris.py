from tetris import *
from matrix import *

class CTetris(Tetris):
	setOfCBlockObjects = 0

	@classmethod
	def init(cls, setOfBlockArrays):
		Tetris.init(setOfBlockArrays)
		CTetris.setOfCBlockObjects = [[0]* Tetris.nBlockDegrees for _ in range(Tetris.nBlockTypes)]

		for i in range(Tetris.nBlockTypes):
			for j in range(Tetris.nBlockDegrees):
				obj = Matrix(setOfBlockArrays[i][j])
				#1과0으로 이루어진 블럭을 1~7중 하나와 0으로 이루어지도록 바꿈, 
				#mulc는 i+1만큼을 각 행에 곱해줌
				#ex [0, 1, 0] -> [0*7, 1*7, 0*7] -> [0, 7, 0]
				obj.mulc(i+1)
				CTetris.setOfCBlockObjects[i][j] = obj
		return
	
	def __init__(self, cy, cx):
		Tetris.__init__(self, cy, cx)
		arrayScreen = self.createArrayScreen()
		self.iCScreen = Matrix(arrayScreen)
		self.oCScreen = Matrix(self.iCScreen)
		return

	def accept(self, key):
		if key >= '0' and key <= '6':
			if self.justStarted == False:
				self.deleteFullLines()
			self.iCScreen = Matrix(self.oCScreen)

		state = Tetris.accept(self, key)
		
		currCBlk = CTetris.setOfCBlockObjects[self.idxBlockType][self.idxBlockDegree]
		tempBlk = self.iCScreen.clip(self.top, self.left, 
									self.top + currCBlk.get_dy(), 
									self.left + currCBlk.get_dx())
		tempBlk = tempBlk + currCBlk

		self.oCScreen = Matrix(self.iCScreen)
		self.oCScreen.paste(tempBlk, self.top, self.left)
		return state
		
	def deleteFullLines(self):
		nDeleted = 0
		nScanned = self.currBlk.get_dy()

		if self.top + self.currBlk.get_dy() - 1 >= self.iScreenDy:
			nScanned -= (self.top + self.currBlk.get_dy() - self.iScreenDy)

		zero = Matrix([[ 0 for x in range(0, (self.iScreenDx - 2*Tetris.iScreenDw))]])
		for y in range(nScanned - 1, -1, -1):
			cy = self.top + y + nDeleted
			line = self.oScreen.clip(cy, 0, cy+1, self.oScreen.get_dx())
			if line.sum() == self.oScreen.get_dx():
				### Tetris screen
				temp = self.oScreen.clip(0, 0, cy, self.oScreen.get_dx())
				self.oScreen.paste(temp, 1, 0)
				self.oScreen.paste(zero, 0, Tetris.iScreenDw)

				### CTetris screen
				temp = self.oCScreen.clip(0, 0, cy, self.oCScreen.get_dx())
				self.oCScreen.paste(temp, 1, 0)
				self.oCScreen.paste(zero, 0, Tetris.iScreenDw)

				nDeleted += 1
		return

### end of class CTetris():

