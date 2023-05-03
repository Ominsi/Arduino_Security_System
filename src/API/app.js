const express = require('express');
const nodemailer = require('nodemailer');
const app = express()
const port = 3000;

// Create email transporter with the senders information
const transporter = nodemailer.createTransport({
    service: 'EMAIL SERVICE',
    auth: {
        user: 'SENDER EMAIL',
        pass: 'EMAIL PASSWORD'
    }
});

// Create default mail information of sender and subject, that do not change.
const mailDefaults = {
    from: 'SENDER EMAIL',
    subject: 'Security Alert'
};

// Endpoint Called 
app.post('/:email', (req, res) => {
    const mailOptions = mailDefaults; // Make new array equal to the defaults.
    mailOptions.to = req.params.email; // Add the passed email as the to address.
    mailOptions.text = `Security Alert on ${new Date().toLocaleString()}` // Add a message containing when alert was set off.
    // Send the mail, returning 200 if success and 400 if failure.
    transporter.sendMail(mailOptions, (error, info) => { 
        if (error) {
            res.status(400);
            res.send(error);
        } else {
            res.status(200);
            res.send("Email sent" + info.response);
        }
    })
});

app.listen(port, '10.0.0.165', () => {
    console.log(`App listening on port ${port}`);
})