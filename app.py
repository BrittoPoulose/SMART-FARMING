from json import load
from unittest import result
from flask import Flask, request,render_template
import pickle

path='model/britto-model.pkl'
model=pickle.load(open(path,'rb'))

app=Flask(__name__)

@app.route("/")
def home():
    return render_template("index.html")

@app.route("/predict1",methods=['GET','POST'])
def predict():
    if request.method=='POST':
        ph=int(request.form['ph'])
        water=int(request.form['water'])
        temp=request.form['temp']
        hum=request.form['hum']

        res=model.predict([[water,ph,temp,hum]])
        print(res[0])
        return render_template("predict1.html",result=res[0])

if __name__ == '__main__':
    app.run(debug=True)