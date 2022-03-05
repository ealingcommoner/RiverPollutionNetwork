library(dplyr)
library(ggplot2)
# Read in and clean data
df<- read.csv('sensors6.csv', skip=50)
head(df)
colnames(df)<- c("DateTime", "ID","Sensor", "v")
df<-na.omit(df)

#Clean up times and convert to mins since start time
start<- df$DateTime[1]
df$DateTime<- as.POSIXct(df$DateTime)
start<- df$DateTime[1]
df<- df %>% mutate(time= difftime(DateTime, start))

df$time<- as.numeric(df$time)/60

#Plot battery life
m<- ggplot(df, aes(x=time, y=v)) + 
        geom_point()+
        geom_smooth(method=lm, alpha=0.3)

#Build linear regression model and make prediction 
model<- lm(time~v, data=df)
summary(model)
p <-  as.data.frame(0)
colnames(p) <- c("v")

pred<- predict(model, newdata = p)

#Convert prediction to days and extrapolate to full charge
print(pred/(60*24)*(100/df$v[1]))