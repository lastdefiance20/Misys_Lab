'''
https://ychae-leah.tistory.com/78
https://altongmon.tistory.com/703
https://newtoynt.tistory.com/entry/python-timeout-only-linux
'''

from tetris import *
from random import *

#운영체제용?
import os
#sys, tty, termios는 키보드 키 감지용 모듈(우분투, 맥용)
import sys
import tty
import termios
#알람(시간제한용)
import signal

#스레딩?(병렬동작용)
import threading

#시간
import time

##############################################################
### Data model related code
############################################################## 

#테트리스 블럭 회전, m_array는 테트리스 블럭 하나의 array를 넘겨 받음
def rotate(m_array):
	#array의 사이즈 (n by m matrix 사이즈 감지)
    size = len(m_array)
	#empty array 생성
    r_array = [[0] * size for _ in range(size)]

	#array size 만큼 rotate 실행 (시계방향으로 90도 회전)
    for y in range(size):
        for x in range(size):
            r_array[x][size-1-y] = m_array[y][x]

	#회전된 array return
    return r_array

def initSetOfBlockArrays():
	#nBlocks 글로벌 선언
    global nBlocks

	#arrayBlks = 테트리스 블럭 모음
    arrayBlks = [ [ [ 0, 0, 1, 0 ],     # I shape
                    [ 0, 0, 1, 0 ],     
                    [ 0, 0, 1, 0 ],     
                    [ 0, 0, 1, 0 ] ],   
                  [ [1, 0, 0],          # J shape
                    [1, 1, 1],          
                    [0, 0, 0] ],
                  [ [0, 0, 1],          # L shape
                    [1, 1, 1],          
                    [0, 0, 0] ],        
                  [ [1, 1],             # O shape
                    [1, 1] ],           
                  [ [0, 1, 1],          # S shape
                    [1, 1, 0],          
                    [0, 0, 0] ],
                  [ [0, 1, 0],          # T shape    
                    [1, 1, 1],          
                    [0, 0, 0] ],
                  [ [1, 1, 0],          # Z shape
                    [0, 1, 1],          
                    [0, 0, 0] ]         
                ]

	#arrayBlks에 있는 블럭의 개수를 nBlocks에 저장
    nBlocks = len(arrayBlks)

	#setOfBlockArrays에는 [[0, 0, 0, 0], [0, 0, 0, 0], ~ 식으로 [0,0,0,0]이 nBlock만큼 저장되어 있음
    setOfBlockArrays = [[0] * 4 for _ in range(nBlocks)]

	#idxBlockType는 nBlock에서 for로 하나씩 뽑아온 arrayBlks의 index
    for idxBlockType in range(nBlocks):
		#temp_array에는 테트리스 블럭 중 idxBlockType index에 포함되는 블럭 하나의 전체 array가 저장됨
        temp_array = arrayBlks[idxBlockType]
		#setOfBlockArrays에 기본 state를 저장함
        setOfBlockArrays[idxBlockType][0] = temp_array
		#블럭을 한번씩 계속 rotate시킨 청사진을 setOfBlockArrays에 저장하여 블럭이 가능한 모든 state를 표현
		#ex) ㅗ가 저장된 setOfBlockArrays = [[ㅗ, ㅏ, ㅜ, ㅓ], [다른 블럭 4가지 state], ~]
        for idxBlockDegree in range(1,4):
            temp_array = rotate(temp_array)
            setOfBlockArrays[idxBlockType][idxBlockDegree] = temp_array

	#모든 블럭의 모든 state가 저장된 setOfBlockArrays를 반환함
	#setOfBlockArrays[블럭의 index(0~6 7개)][블럭의 state(0~3 4개, 1->2 right rotate, 1->0 left rotate)]
    return setOfBlockArrays

#board = 테트리스 보드, key = 키보드 키 입력
def processKey(board, key):
	#nBlocks 글로벌 선언
	global nBlocks

	#보드에 key값 입력
	state = board.accept(key)

	#테트리스 보드 프린트
	printScreen(board)
    
	#state가 새로운 테트리스 블럭을 원하지 않으면 return
	if state != TetrisState.NewBlock:
		return state

	#블럭타입 인덱스 랜덤으로 하나 뽑음
	idxBlockType = randint(0, nBlocks-1)

	#테트리스 블럭 타입을 key에 저장
	#ex) 테트리스 블럭 타입 = 3, key = '03'
	#key = '0' + str(idxBlockType)
	key = str(idxBlockType)

	#board는 Tetris class이다.
	#Tetris class에서 accept(key) 함수는 테트리스 블럭 타입을 받으면
	#블럭 타입에 맞는 새로운 블럭을 위에 꺼내게 된다.
	state = board.accept(key)

	#테트리스 보드 프린트
	printScreen(board)

	if state != TetrisState.Finished:
		return state
	
	return state

##############################################################
### UI code
##############################################################

#clearScreen 함수는 우리가 쓰던 consol log를 지워주는 역할을 한다.
#ex) 1이 출력되어 있는데 clearscreen을 하면 출력된 내용이 사라진다.
#없으면 위에 하던 테트리스 board가 쭉 남게 된다.
def clearScreen(numlines=100):
	'''
	>>> import os 
	>>> os.system('cls') # window 
	>>> os.system('clear') # linux, mac
	'''

	#posix = 리눅스
	if os.name == 'posix':
		os.system('clear')

	#nt, dos, ce = 윈도우
	elif os.name in ['nt', 'dos', 'ce']:
		os.system('CLS')
	else:
		#엔터로 지운것처럼 해줌
		print('\n' * numlines)
	return

#printScreen함수는 보드를 출력해주는 역할을 한다.
def printScreen(board):
	#출력된 보드를 지운다.
	clearScreen()

	array = board.oScreen.get_array()

	for y in range(board.oScreen.get_dy()-Tetris.iScreenDw):
		#한줄한줄 출력한다
		line = ''
		for x in range(Tetris.iScreenDw, board.oScreen.get_dx()-Tetris.iScreenDw):
			if array[y][x] == 0:
				line += '□'
			elif array[y][x] == 1:
				line += '■'
			else:
				line += 'XX'
		print(line)

	#엔터로 공백 생성
	print()
	return

queue = list()
cv = threading.Condition()
#게임이 종료되었는지 알리는 status 설정
isGameDone = False

#우분투용 getchar()
def getChar():
	#sys.stdin.read에는 \n 엔터라는 버퍼가 남는다는 점이 있다.
	#때문에 원래는 우리가 하나 입력하고 엔터를 쳐야 했다면, 이제는
	#우리가 char 1개를 입력하면 바로 입력되도록 getChar에 구성되어 있다?
	ch = sys.stdin.read(1)
	return ch

#https://velog.io/@wltjs10645/Python-thread2
'''
데이터의 무결성을 보장하기 위해 Lock 을 사용한다❗️
데이터를 수정하기 전에 락을 잡아 다른 스레드에서 값을 수정하지 못하게 막는 것.
값을 수정하고 나면 락을 풀어 다른 스레드에서 이용할 수 있게 됨
'''

#키프로듀서 - 키보드에 키를 입력하면 queue라는 리스트에 우리가 입력한 키를 저장해준다.
class KeyProducer(threading.Thread):
	def run(self):
		global isGameDone

		#게임이 끝나지 않았다면
		while not isGameDone:
			try:
				#키보드에서 char를 하나 입력받음
				key = getChar()
			except:
				#char 입력에서 에러가 발생하면 get~ 에러문을 print 하고 break를 하여 종료함
				isGameDone = True
				print('getChar() wakes up!!')
				break
			#queue에 단독으로 접근할 권한을 얻는다.
			cv.acquire()
			#queue 리스트에 입력받은 key를 append하여 추가한다.
			queue.append(key)
			#wait 하고 있는 소비자를 깨운다.
			cv.notify()
			#queue에 단독으로 접근할 권한을 반납한다.
			cv.release()
			#입력받은 key가 q이면 게임을 종료한다.
			if key == 'q':
				isGameDone = True
				break
		return

#타임아웃프로듀서
#분석해보니까 1초 이후 s를 누르게 하여 아래로 내려가도록 함
#메인에서 좌우로 움직이면 계속 안내려가지만 메인스레딩에서는 스레딩을 이용하여 좌우로 움직여도 1초 이후에는 무조건 내려가도록 병렬로 구현함
class TimeOutProducer(threading.Thread):
	def run(self):
		#게임이 끝나지 않았다면
		while not isGameDone:
			#1초 기다림
			time.sleep(1)
			#queue에 작업을 수행하기 전에 queue에 단독으로 접근할 권한을 얻는다.
			cv.acquire()
			#queue 리스트에 테트리스가 내려갈 수 있도록 's' 키를 추가한다.
			queue.append('s')
			#wait 하고 있는 소비자를 깨운다.
			cv.notify()
			#queue에 단독으로 접근할 권한을 반납한다.
			cv.release()
		return
'''
queue = list()
cv = threading.Condition()
'''
#컨수머
class Consumer(threading.Thread):
	def run(self):
		global isGameDone

		setOfBlockArrays = initSetOfBlockArrays()

		#테트리스 블럭 청사진을 입력함
		Tetris.init(setOfBlockArrays)
		#테트리스 보드 크기를 설정함
		board = Tetris(20, 15)

		#처음 시작할때 나올 블럭을 랜덤으로 정하고 출력함
		idxBlockType = randint(0, nBlocks-1)
		#key = '0' + str(idxBlockType)
		key = str(idxBlockType)
		state = board.accept(key)
		printScreen(board)

		#게임이 끝나지 않았을때
		while not isGameDone:
			#queue에 단독으로 접근할 권한을 얻는다.
			cv.acquire()
			#queue에 아무것도 들어있지 않을 때, 입력받아서 queue에 값이 들어올때까지 기다린다
			while len(queue) < 1:
				# 큐에 데이터가 없으니 기다린다.
				# 단독 접근 권한을 잠시 해제한다
				# notify로 깨어날때마다 체크한다.
				cv.wait()
			#큐에 데이터가 있으며, 자신만이 다룰 수 있는 상태이다

			#queue에 저장된 key값을 하나 pop해서 queue에서 빼온다
			key = queue.pop(0)
			#queue에 접근이 끝났으니, 포기한다.
			cv.release()
			
			#key 값이 q이면 종료한다.
			if key == 'q':
				state = TetrisState.Finished
				print('Game aborted...')
				break

			#key값을 processKey에 전달해서 key값에 맞는 액션을 tetris에서 취할 수 있도록 한다.
			state = processKey(board, key)

			#state값이 finished이면 종료한다.
			if state == TetrisState.Finished:
				isGameDone = True
				print('Game Over!!!')
				os.kill(os.getpid(), signal.SIGINT)
				break
		return

#시그널 보냄
def signal_handler(num, stack):
	print('signal_handler called!!')
	raise RuntimeError

#main 함수
if __name__ == "__main__":
	global fd
	global old_settings

	signal.signal(signal.SIGINT, signal_handler)

	#쓰레드를 추가해서 병렬로 동작하도록 함
	threads = list()
	threads.append(Consumer()) #컨슈머 추가
	threads.append(KeyProducer()) #키프로듀서 추가
	threads.append(TimeOutProducer()) #타임아웃프로듀서 추가

	fd = sys.stdin.fileno()
	old_settings = termios.tcgetattr(fd)
	tty.setcbreak(sys.stdin.fileno())

	#쓰레드 동작 start
	for th in threads:
		th.start()
	
	#오류 종류에 상관없이 오류가 발생하면 except 블록을 수행한다.
	#.join()을 실행할때 3개의 병렬로 동작중인 프로그램 중 하나에서 break문이 발생하게 되면
	#except문으로 넘어가게 되어 th~가 print되고 프로그램이 종료하게 된다.
	for th in threads:
		try:
			th.join()
		except:
			print('th.join() wakes up!!')
	
	#프로그램이 종료됨(except에 걸림)
	termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
	print('Program terminated...')

### end of main.py

