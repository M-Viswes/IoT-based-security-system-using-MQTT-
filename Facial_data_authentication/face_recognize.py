import cv2, numpy, os
import mysql.connector
import Adafruit_CharLCD as LCD

db = mysql.connector.connect(
  host="localhost",
  user="admin",
  passwd="user",
  database="attendance"
)

cursor = db.cursor()
lcd = LCD.Adafruit_CharLCD(4, 24, 23, 17, 18, 22, 16, 2, 4)
lcd.message('Recognizing face ...')
size = 4
haar_file = 'haarcascade_frontalface_default.xml'
datasets = 'data'
print('Training...')
(images, labels, names, id) = ([], [], {}, 0)
for (subdirs, dirs, files) in os.walk(datasets):
    for subdir in dirs:
        names[id] = subdir
        subjectpath = os.path.join(datasets, subdir)
        for filename in os.listdir(subjectpath):
            path = subjectpath + '/' + filename
            
            label = id
            images.append(cv2.imread(path, 0))
            labels.append(int(label))
            #print(labels)
        id += 1
(width, height) = (130, 100)

(images, labels) = [numpy.array(lis) for lis in [images, labels]]

#print(images, labels)
#model = cv2.face.LBPHFaceRecognizer_create()
model =  cv2.face.FisherFaceRecognizer_create()
model.train(images, labels)

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades+haar_file)
webcam = cv2.VideoCapture(0)
cnt=0
while True:
    (_, im) = webcam.read()
    gray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.3, 5)
    for (x,y,w,h) in faces:
        cv2.rectangle(im,(x,y),(x+w,y+h),(255,255,0),2)
        face = gray[y:y + h, x:x + w]
        face_resize = cv2.resize(face, (width, height))

        prediction = model.predict(face_resize)
        cv2.rectangle(im, (x, y), (x + w, y + h), (0, 255, 0), 3)

        if prediction[1]<800:
            # cv2.putText(im,'%s - %.0f' % (names[prediction[0]],prediction[1]),(x-10, y-10), cv2.FONT_HERSHEY_PLAIN,1,(255, 0, 0))
            cursor.execute("Select user_id FROM attendanceall WHERE user_id=" + str(prediction[0] - 1))
            print("ID IS :",prediction[0])
            result = cursor.fetchone()

            
            print("Authorization Sucess !!")
            lcd.message('Door is open !')
            cursor.execute(f"UPDATE attendanceall SET in_Admin = \"YES\" WHERE user_id = \"{str(prediction[0])}\";")
            db.commit()
           
            
            cnt=0
        
        else:
            cnt+=1
            # cv2.putText(im,'Unknown',(x-10, y-10), cv2.FONT_HERSHEY_PLAIN,1,(0, 255, 0))

            if(cnt>100):
                print("Unknown Person")
                
                cnt=0
    
    #cv2.imshow('OpenCV', im)
    key = cv2.waitKey(10)
    if key == 27:
        break

webcam.release()
cv2.destroyAllWindows()