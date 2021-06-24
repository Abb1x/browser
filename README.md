# AWB: Abbix's web browser
pls send help

AWB is a simple web browser written for learning purposes in C++, it features a custom engine named *orca* rendered using SFML.

## Acknowledgments
Won't be anything big, just a toy browser.
Features (right now):
- HTTP(s)
- CSS parsing
- HTML parsing
- WIP Engine 
- Telescope support
- Telescope's markup language: star
## Supported CSS
- font-size (numbers only) 
- color (names and 6 digit hex)

## Screenshots
Here is AWB rendering the following code:
```html
<html>
  <head>
    <style>
      html
      {
	  color: #8FBCBB;
      } 

      p
      {
	  font-size: 40;
      }
      
      .myclass
      {
	  color: red;  
      }
      
    </style>
  </head>
<body>
  <p> Welcome to AWB (Abbix's web browser) (I will probably change the name)! </p>
  <p class="myclass"> Hey! This is a demo HTML page! </p>
</body>
</html>
```
![image](https://user-images.githubusercontent.com/54687179/116110831-0c77be00-a684-11eb-935d-d75b79784726.png)

<br>

**It won't support JS.**
