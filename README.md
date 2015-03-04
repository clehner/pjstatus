# pjstatus
**update the status display of a networked HP printer**

![photo showing printer with custom status](https://cloud.githubusercontent.com/assets/95347/6493120/97746ea6-c286-11e4-912d-20097d9b16ef.jpg)


## Usage
```
Usage: ./pjstatus hostname [status...]
```

## Example

get the current message
```
$ pjstatus printera
Processing job
```

set our message
```
$ pjstatus printera Hi I am a printer
```

read back our message
```
$ pjstatus printera
Hi I am a printer
```

## References

[PJL Technical Reference Manual](http://h20565.www2.hp.com/hpsc/doc/public/display?docId=emr_na-bpl13208)

## License

MIT
